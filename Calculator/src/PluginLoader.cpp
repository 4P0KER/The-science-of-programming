#include "PluginLoader.h"
#include "Calculator.h"
#include "IOperation.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

PluginLoader::~PluginLoader() {
    unloadAll();
}

bool PluginLoader::loadPluginsFromDirectory(const std::string& directory, Calculator& calculator) {
    try {
        if (!fs::exists(directory)) {
            std::cout << "Plugins directory not found: " << directory << std::endl;
            return false;
        }

        bool loadedAny = false;
        
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.path().extension() != ".dll") continue;
            
            HMODULE hDll = LoadLibraryA(entry.path().string().c_str());
            if (!hDll) {
                DWORD error = GetLastError();
                std::cerr << "Failed to load " << entry.path().filename() 
                         << " (Error " << error << ")" << std::endl;
                continue;
            }

            auto createFunc = reinterpret_cast<IOperation*(*)()>(
                GetProcAddress(hDll, "createOperation"));
            
            if (!createFunc) {
                std::cerr << "No 'createOperation' in: " << entry.path().filename() << std::endl;
                FreeLibrary(hDll);
                continue;
            }

            try {
                IOperation* operation = createFunc();
                if (!operation) {
                    std::cerr << "Plugin returned null: " << entry.path().filename() << std::endl;
                    FreeLibrary(hDll);
                    continue;
                }

                if (auto* unary = dynamic_cast<IUnaryFunction*>(operation)) {
                    std::string name = unary->getName();
                    if (!calculator.hasFunction(name)) {
                        calculator.registerFunction(name, UnaryFunctionPtr(unary));
                        loadedAny = true;
                        loadedLibraries.push_back(hDll);
                        std::cout << "Loaded function: " << name << std::endl;
                    } else {
                        std::cerr << "Function already exists: " << name << std::endl;
                        delete unary;
                        FreeLibrary(hDll);
                    }
                }
                else if (auto* binary = dynamic_cast<IBinaryOperator*>(operation)) {
                    char symbol = binary->getSymbol();
                    if (!calculator.hasBinaryOperator(symbol)) {
                        calculator.registerBinaryOperator(symbol, BinaryOperatorPtr(binary));
                        loadedAny = true;
                        loadedLibraries.push_back(hDll);
                        std::cout << "Loaded operator: '" << symbol << "'" << std::endl;
                    } else {
                        std::cerr << "Operator already exists: '" << symbol << "'" << std::endl;
                        delete binary;
                        FreeLibrary(hDll);
                    }
                }
                else {
                    std::cerr << "Unknown operation type in: " << entry.path().filename() << std::endl;
                    delete operation;
                    FreeLibrary(hDll);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error loading plugin " << entry.path().filename() 
                         << ": " << e.what() << std::endl;
                FreeLibrary(hDll);
            }
        }

        if (loadedAny) {
            std::cout << "=== Plugin loading complete ===" << std::endl;
        }
        return loadedAny;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in plugin loader: " << e.what() << std::endl;
        return false;
    }
}

void PluginLoader::unloadAll() {
    for (HMODULE hDll : loadedLibraries) {
        FreeLibrary(hDll);
    }
    loadedLibraries.clear();
}