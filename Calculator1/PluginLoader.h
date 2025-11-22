#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include <vector>

class Calculator;

class PluginLoader {
private:
    std::vector<HMODULE> loadedLibraries;

public:
    ~PluginLoader();
    bool loadPluginsFromDirectory(const std::string& directory, Calculator& calculator);
    void unloadAll();
};