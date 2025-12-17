#ifndef WRAPPER_H
#define WRAPPER_H

#include <functional>
#include <any>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <stdexcept>
#include <utility>
#include <memory>
#include <sstream>

// Базовый интерфейс для всех команд
class ICommand {
public:
    virtual ~ICommand() = default;
    
    virtual std::any execute(const std::map<std::string, std::any>& arguments) = 0;
    
    virtual std::vector<std::string> get_parameter_names() const = 0;
};

// Обертка для методов класса с произвольной сигнатурой
template<typename ClassType, typename ReturnType, typename... Args>
class MethodWrapper : public ICommand {
private:
    ClassType* instance_;
    std::function<ReturnType(Args...)> method_;
    std::vector<std::string> param_names_;
    std::map<std::string, std::any> default_values_;
    std::unordered_map<std::string, size_t> param_index_;
    std::vector<std::string> param_types_;
    
    // Извлечение типа из кортежа
    template<size_t Index>
    using ArgType = typename std::tuple_element<Index, std::tuple<Args...>>::type;
    
    // Получение имени типа в виде строки
    template<typename U>
    static std::string get_type_name() {
        return typeid(U).name();
    }
    
    // Проверяет типы значений по умолчанию при создании обертки
    template<size_t... Is>
void validate_defaults_impl(std::index_sequence<Is...>) {
    if constexpr (sizeof...(Args) > 0) {
        constexpr size_t count = sizeof...(Args);
        [&]<size_t... Idxs>(std::index_sequence<Idxs...>) {
            auto validate = [&](size_t idx) {
                const std::string& param_name = param_names_[idx];
                auto it = default_values_.find(param_name);
                if (it != default_values_.end()) {
                    try {
                        using ExpectedType = typename std::tuple_element<idx, std::tuple<Args...>>::type;
                        std::ignore = std::any_cast<ExpectedType>(it->second);
                    } catch (const std::bad_any_cast&) {
                        throw std::runtime_error(
                            "Default value type mismatch for parameter '" + 
                            param_name + "'. Expected: " + param_types_[idx]
                        );
                    }
                }
            };
            (validate(Idxs), ...);
        }(std::make_index_sequence<count>{});
    }
}
    
    // Валидирует значения по умолчанию
    void validate_defaults() {
        if (!default_values_.empty()) {
            validate_defaults_impl(std::make_index_sequence<sizeof...(Args)>{});
        }
    }
    
    template<size_t Index>
    ArgType<Index> get_argument(const std::map<std::string, std::any>& args) const {
        const std::string& param_name = param_names_[Index];
        
        auto it = args.find(param_name);
        if (it != args.end()) {
            try {
                return std::any_cast<ArgType<Index>>(it->second);
            } catch (const std::bad_any_cast&) {
                std::stringstream ss;
                ss << "Type mismatch for argument '" << param_name 
                   << "'. Expected: " << param_types_[Index];
                throw std::runtime_error(ss.str());
            }
        }
        
        auto default_it = default_values_.find(param_name);
        if (default_it != default_values_.end()) {
            try {
                return std::any_cast<ArgType<Index>>(default_it->second);
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error("Default value type mismatch: " + param_name);
            }
        }
        
        throw std::runtime_error("Missing required argument: " + param_name);
    }
    
    template<size_t... Indices>
    auto make_tuple_args(const std::map<std::string, std::any>& args, 
                        std::index_sequence<Indices...>) const {
        return std::make_tuple(get_argument<Indices>(args)...);
    }
    
    // Инициализация информации о параметрах
    void initialize(const std::map<std::string, std::any>& defaults) {
        if (defaults.empty()) {
            param_names_.clear();
            for (size_t i = 0; i < sizeof...(Args); ++i) {
                param_names_.push_back("arg" + std::to_string(i + 1));
            }
        } else {
            if (defaults.size() != sizeof...(Args)) {
                throw std::runtime_error(
                    "Parameter count mismatch. Expected " + 
                    std::to_string(sizeof...(Args)) + 
                    " parameters, got " + 
                    std::to_string(defaults.size())
                );
            }
            
            param_names_.clear();
            for (const auto& [name, _] : defaults) {
                param_names_.push_back(name);
            }
        }
        
        // Инициализация индексов для быстрого поиска
        for (size_t i = 0; i < param_names_.size(); ++i) {
            param_index_[param_names_[i]] = i;
        }
        
        // Инициализация информации о типах параметров
        if constexpr (sizeof...(Args) > 0) {
            param_types_ = {get_type_name<Args>()...};
        }
        
        // Сохранение значений по умолчанию
        default_values_ = defaults;
    }
    
    void check_required_arguments(const std::map<std::string, std::any>& arguments) const {
        if (default_values_.empty()) {
            for (const auto& name : param_names_) {
                if (arguments.find(name) == arguments.end()) {
                    throw std::runtime_error("Missing required argument: " + name);
                }
            }
        }
    }

public:
    // Конструктор для неконстантных методов
    template<typename MethodPtr>
    MethodWrapper(ClassType* instance, MethodPtr method,
                  const std::map<std::string, std::any>& defaults = {})
        : instance_(instance) {
        
        if (!instance_) {
            throw std::runtime_error("Instance pointer cannot be null");
        }
        
        method_ = [this, method](Args... args) -> ReturnType {
            return (instance_->*method)(args...);
        };
        
        initialize(defaults);
        validate_defaults();  
    }
    
    // Конструктор для константных методов
    MethodWrapper(ClassType* instance, 
                  ReturnType (ClassType::*method)(Args...) const,
                  const std::map<std::string, std::any>& defaults = {})
        : instance_(instance) {
        
        if (!instance_) {
            throw std::runtime_error("Instance pointer cannot be null");
        }
        
        method_ = [this, method](Args... args) -> ReturnType {
            return (instance_->*method)(args...);
        };
        
        initialize(defaults);
        validate_defaults();
    }

    std::any execute(const std::map<std::string, std::any>& arguments) override {
        // Проверка на дубликаты аргументов
        std::map<std::string, int> arg_count;
        for (const auto& [name, _] : arguments) {
            // Проверка на неизвестные аргументы
            if (param_index_.find(name) == param_index_.end()) {
                throw std::runtime_error("Unknown argument: " + name);
            }
            
            if (++arg_count[name] > 1) {
                throw std::runtime_error("Duplicate argument: " + name);
            }
        }
        
        check_required_arguments(arguments);
        
        if constexpr (sizeof...(Args) == 0) {
            // Метод без параметров
            return method_();
        } else {
            // Метод с параметрами
            auto args_tuple = make_tuple_args(arguments, 
                                             std::make_index_sequence<sizeof...(Args)>{});
            return std::apply(method_, args_tuple);
        }
    }
    
    std::vector<std::string> get_parameter_names() const override {
        return param_names_;
    }

    std::vector<std::string> get_parameter_types() const {
        return param_types_;
    }
};

// Вспомогательная функция для создания обертки
template<typename ClassType, typename ReturnType, typename... Args>
std::unique_ptr<ICommand> create_wrapper(ClassType* instance,
                                         ReturnType (ClassType::*method)(Args...),
                                         const std::map<std::string, std::any>& defaults = {}) {
    return std::make_unique<MethodWrapper<ClassType, ReturnType, Args...>>(
        instance, method, defaults);
}

template<typename ClassType, typename ReturnType, typename... Args>
std::unique_ptr<ICommand> create_wrapper(ClassType* instance,
                                         ReturnType (ClassType::*method)(Args...) const,
                                         const std::map<std::string, std::any>& defaults = {}) {
    return std::make_unique<MethodWrapper<ClassType, ReturnType, Args...>>(
        instance, method, defaults);
}

#endif 
