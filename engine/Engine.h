#ifndef ENGINE_H
#define ENGINE_H

#include "Wrapper.h"
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <vector>

class Engine {
private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> commands_;
    
public:
    void register_command(const std::string& name, std::unique_ptr<ICommand> command) {
        if (name.empty()) {
            throw std::runtime_error("Command name cannot be empty");
        }
        
        if (!command) {
            throw std::runtime_error("Command cannot be null");
        }
        
        if (commands_.find(name) != commands_.end()) {
            throw std::runtime_error("Command already registered: " + name);
        }
        
        commands_[name] = std::move(command);
    }
    
    std::any execute(const std::string& name, 
                     const std::map<std::string, std::any>& arguments = {}) {
        if (name.empty()) {
            throw std::runtime_error("Command name cannot be empty");
        }
        
        auto it = commands_.find(name);
        if (it == commands_.end()) {
            throw std::runtime_error("Command not found: " + name);
        }
        
        return it->second->execute(arguments);
    }
    
    // Типобезопасное выполнение
    template<typename ResultType>
    ResultType execute_as(const std::string& name,
                          const std::map<std::string, std::any>& arguments = {}) {
        std::any result = execute(name, arguments);
        
        try {
            return std::any_cast<ResultType>(result);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Return type mismatch for command: " + name);
        }
    }
    
    bool has_command(const std::string& name) const {
        return commands_.find(name) != commands_.end();
    }
    
    bool remove_command(const std::string& name) {
        return commands_.erase(name) > 0;
    }
    
    void clear() {
        commands_.clear();
    }

    std::vector<std::string> get_command_list() const {
        std::vector<std::string> result;
        result.reserve(commands_.size());
        
        for (const auto& [name, _] : commands_) {
            result.push_back(name);
        }
        
        return result;
    }
    
    size_t command_count() const {
        return commands_.size();
    }

    std::vector<std::string> get_command_params(const std::string& name) const {
        auto it = commands_.find(name);
        if (it == commands_.end()) {
            throw std::runtime_error("Command not found: " + name);
        }
        
        return it->second->get_parameter_names();
    }
};

#endif 
