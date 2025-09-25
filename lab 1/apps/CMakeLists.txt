# Applications
project(employee_apps)

# Creator application
add_executable(employee_creator creator/creator.cpp)
target_link_libraries(employee_creator PRIVATE employee_core)
target_include_directories(employee_creator PRIVATE ${CMAKE_SOURCE_DIR}/core)

set_target_properties(employee_creator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME creator
)

# Reporter application
add_executable(employee_reporter reporter/reporter.cpp)
target_link_libraries(employee_reporter PRIVATE employee_core)
target_include_directories(employee_reporter PRIVATE ${CMAKE_SOURCE_DIR}/core)

set_target_properties(employee_reporter PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME reporter
)

# Main application
add_executable(employee_main main_app/main_app.cpp)
target_link_libraries(employee_main PRIVATE employee_core)
target_include_directories(employee_main PRIVATE ${CMAKE_SOURCE_DIR}/core)

set_target_properties(employee_main PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME main_app
)

# Установка приложений
install(TARGETS 
    employee_creator 
    employee_reporter 
    employee_main
    DESTINATION bin
)

# Мета-цель для всех приложений
add_custom_target(apps DEPENDS employee_creator employee_reporter employee_main)