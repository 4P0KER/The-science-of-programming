#include <iostream>
#include <string>
#include <memory>
#include <filesystem>
#include "Calculator.h"
#include "PluginLoader.h"

namespace fs = std::filesystem;

std::string getPluginsDirectory() {
    fs::path cwd = fs::current_path();
    
    std::vector<fs::path> possiblePaths = {
        cwd / "plugins",
        cwd / "bin" / "plugins",
        cwd.parent_path() / "plugins"
    };
    
    for (const auto& path : possiblePaths) {
        if (fs::exists(path) && fs::is_directory(path)) {
            return path.string();
        }
    }
    
    fs::path defaultPath = cwd / "plugins";
    fs::create_directories(defaultPath);
    std::cout << "Created plugins directory: " << defaultPath << std::endl;
    
    return defaultPath.string();
}

int main() {
    std::cout << "Calculator with Plugin System" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "Type 'quit' to exit, 'help' for help" << std::endl << std::endl;

    auto calculator = std::make_unique<Calculator>();
    auto pluginLoader = std::make_unique<PluginLoader>();

    std::string pluginsPath = getPluginsDirectory();
    std::cout << "Plugins directory: " << pluginsPath << std::endl;

    std::cout << "Loading plugins..." << std::endl;
    if (!pluginLoader->loadPluginsFromDirectory(pluginsPath, *calculator)) {
        std::cout << "No plugins loaded. Only basic operations (+ - * /) are available." << std::endl;
        std::cout << "Place plugin DLLs in: " << pluginsPath << std::endl;
    }

    calculator->listFunctions();
    calculator->listBinaryOperators();
    std::cout << std::endl;

    std::string input;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, input)) break;
        
        if (input.empty()) continue;
        if (input == "quit" || input == "exit") break;
        if (input == "help") {
            std::cout << "Available commands:" << std::endl;
            std::cout << "  quit, exit - Exit program" << std::endl;
            std::cout << "  help      - Show this help" << std::endl;
            std::cout << "  funcs     - List available functions" << std::endl;
            std::cout << "  ops       - List available operators" << std::endl;
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  2 + 3 * 4" << std::endl;
            std::cout << "  sin(90) + ln(2.71828)" << std::endl;
            std::cout << "  2 ^ 4" << std::endl;
            continue;
        }
        if (input == "funcs") {
            calculator->listFunctions();
            continue;
        }
        if (input == "ops") {
            calculator->listBinaryOperators();
            continue;
        }

        try {
            double result = calculator->evaluate(input);
            std::cout << "= " << result << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}