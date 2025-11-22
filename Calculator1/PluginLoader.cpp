#include "PluginLoader.h"
#include "Calculator.h"
#include "ICalcFunction.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

PluginLoader::~PluginLoader() {
    unloadAll();
}

bool PluginLoader::loadPluginsFromDirectory(const std::string& directory, Calculator& calculator) {

    try {
        if (!fs::exists(directory)) {
            std::cerr << "ERROR: Directory does not exist: " << directory << std::endl;
            return false;
        }

        bool loadedAny = false;

        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.path().extension() == ".dll") {

                HMODULE hDll = LoadLibraryA(entry.path().string().c_str());
                if (!hDll) {
                    DWORD error = GetLastError();
                    std::cerr << "ERROR: Failed to load DLL (Error " << error << ")" << std::endl;
                    continue;
                }

                bool pluginLoaded = false;

                // Пробуем загрузить бинарный оператор
                auto createBinaryOp = reinterpret_cast<IBinaryOperator * (*)()>(
                    GetProcAddress(hDll, "createBinaryOperator"));

                if (createBinaryOp) {
                    try {
                        IBinaryOperator* rawOp = createBinaryOp();
                        BinaryOperatorPtr op(rawOp);
                        char symbol = op->getSymbol();
                        calculator.registerBinaryOperator(symbol, op);
                        loadedLibraries.push_back(hDll);
                        loadedAny = true;
                        pluginLoaded = true;
                    }
                    catch (const std::exception& e) {
                        std::cerr << "ERROR creating binary operator: " << e.what() << std::endl;
                        FreeLibrary(hDll);
                    }
                }

                // Пробуем загрузить обычную функцию
                if (!pluginLoaded) {
                    auto createFunc = reinterpret_cast<ICalcFunction * (*)()>(
                        GetProcAddress(hDll, "createFunction"));

                    if (createFunc) {
                        try {
                            ICalcFunction* rawFunc = createFunc();
                            CalcFunctionPtr func(rawFunc);
                            std::string funcName = func->getName();
                            calculator.registerFunction(funcName, func);
                            loadedLibraries.push_back(hDll);
                            loadedAny = true;
                            pluginLoaded = true;
                        }
                        catch (const std::exception& e) {
                            std::cerr << "ERROR creating function: " << e.what() << std::endl;
                            FreeLibrary(hDll);
                        }
                    }
                }

                if (!pluginLoaded) {
                    std::cerr << "ERROR: No valid exports found, unloading DLL" << std::endl;
                    FreeLibrary(hDll);
                }
            }
        }

        std::cout << "=== Plugin loading complete ===" << std::endl;
        return loadedAny;
    }
    catch (const std::exception& e) {
        std::cerr << "EXCEPTION in plugin loader: " << e.what() << std::endl;
        return false;
    }
}

void PluginLoader::unloadAll() {
    for (HMODULE hDll : loadedLibraries) {
        FreeLibrary(hDll);
    }
    loadedLibraries.clear();
}