# Tests configuration
project(employee_tests)


# Поиск Catch2
find_package(Catch2 QUIET)

if(NOT Catch2_FOUND)
    # Используем встроенный Catch2 или doctest
    message(STATUS "Catch2 not found, using simple test framework")
    
    # Unit тесты
    add_executable(unit_tests
        unit/test_employee.cpp
        unit/test_creator.cpp
        unit/test_reporter.cpp
        unit/test_main_app.cpp
    )
    
    target_link_libraries(unit_tests PRIVATE employee_core)
    target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/core)
    
    # E2E тесты
    add_executable(e2e_tests e2e/test_e2e.cpp)
    target_link_libraries(e2e_tests PRIVATE employee_core)
    target_include_directories(e2e_tests PRIVATE ${CMAKE_SOURCE_DIR}/core)
    
else()
    # Используем найденный Catch2
    message(STATUS "Using Catch2 for testing")
    
    include(Catch)
    
    # Unit тесты с Catch2
    catch_discover_tests(unit_tests)
    catch_discover_tests(e2e_tests)
endif()

# Флаги компиляции для тестов
target_compile_options(unit_tests PRIVATE 
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

target_compile_options(e2e_tests PRIVATE 
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# Добавляем тесты в CTest
enable_testing()

add_test(NAME employee_unit_tests COMMAND unit_tests)
add_test(NAME employee_e2e_tests COMMAND e2e_tests)

set_tests_properties(employee_unit_tests PROPERTIES LABELS "unit")
set_tests_properties(employee_e2e_tests PROPERTIES LABELS "e2e")

# Мета-цели
add_custom_target(unit-tests DEPENDS unit_tests)
add_custom_target(e2e-tests DEPENDS e2e_tests)

# Цель для запуска всех тестов
add_custom_target(run-unit-tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L unit --output-on-failure
    DEPENDS unit_tests
    COMMENT "Running unit tests..."
)

add_custom_target(run-e2e-tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L e2e --output-on-failure
    DEPENDS e2e_tests
    COMMENT "Running E2E tests..."
)