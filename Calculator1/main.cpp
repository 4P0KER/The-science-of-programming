#include <iostream>
#include <string>
#include <memory>
#include <filesystem>
#include "Calculator.h"
#include "PluginLoader.h"

namespace fs = std::filesystem;

std::string findPluginsDirectory() {
    std::string exePath = fs::absolute("Calculator.exe").string();
    std::string exeDir = fs::path(exePath).parent_path().string();

    std::vector<std::string> possiblePaths = {
        exeDir + "/plugins",  
        "./plugins",
        "plugins",
        "../plugins"
    };

    for (const auto& path : possiblePaths) {
        if (fs::exists(path) && fs::is_directory(path)) {
            return path;
        }
    }

    std::string defaultPath = exeDir + "/plugins";
    fs::create_directory(defaultPath);
    return defaultPath;
}

int main() {
    std::cout << "Calculator with Plugin System" << std::endl;
    std::cout << "=============================" << std::endl;

    auto calculator = std::make_unique<Calculator>();
    auto pluginLoader = std::make_unique<PluginLoader>();

    // Находим или создаем папку с плагинами
    std::string pluginsPath = findPluginsDirectory();

    // Загружаем плагины
    std::cout << "Loading plugins..." << std::endl;
    if (!pluginLoader->loadPluginsFromDirectory(pluginsPath, *calculator)) {
        std::cout << "No plugins loaded. Please place DLL files in: " << pluginsPath << std::endl;
    }

    calculator->listFunctions();
    calculator->listBinaryOperators();
    std::cout << std::endl;

    std::string input;
    while (true) {
        std::cout << "Enter expression (or 'quit' to exit): ";
        std::getline(std::cin, input);

        if (input == "quit") break;
        if (input.empty()) continue;

        try {
            double result = calculator->evaluate(input);
            std::cout << "Result: " << result << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}