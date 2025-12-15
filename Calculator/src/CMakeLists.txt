# Основное приложение
add_executable(calc
    main.cpp
    Calculator.cpp
    PluginLoader.cpp
)

# Подключаем заголовки
target_include_directories(calc PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Устанавливаем выходную директорию для всех конфигураций
set_target_properties(calc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}
)

# Дополнительная команда для создания папки plugins (можно здесь)
add_custom_command(TARGET calc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/plugins
    COMMENT "Creating plugins directory"
)