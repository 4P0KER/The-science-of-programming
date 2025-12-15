cmake_minimum_required(VERSION 3.10)
project(CalculatorWithPlugins)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем подпроекты
add_subdirectory(src)
add_subdirectory(plugin_sources)

# Создаем папку plugins на этапе конфигурации (а не сборки)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)