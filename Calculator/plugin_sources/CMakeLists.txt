# Создаем плагины
add_library(sin SHARED sin_plugin.cpp)
add_library(ln SHARED ln_plugin.cpp)  # Обратите внимание: In_plugin.cpp, а не ln_plugin.cpp!
add_library(log SHARED log_plugin.cpp)
add_library(pow SHARED pow_plugin.cpp)

# Подключаем заголовки для всех плагинов
foreach(lib sin ln log pow)
    target_include_directories(${lib} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    
    # Устанавливаем выходную директорию для ВСЕХ конфигураций
    set_target_properties(${lib} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/plugins
        
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/plugins
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/plugins
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/plugins
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/plugins
    )
    
    # Добавляем POST_BUILD команду для копирования
    add_custom_command(TARGET ${lib} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${lib}>"
            "${CMAKE_BINARY_DIR}/plugins/"
        COMMENT "Copying ${lib}.dll to plugins folder"
    )
endforeach()