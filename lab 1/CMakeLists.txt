
cmake_minimum_required(VERSION 3.15)
project(EmployeeManagementSystem VERSION 1.0.0 LANGUAGES CXX)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Политики CMake
if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

# Опции проекта
option(BUILD_TESTING "Build tests" ON)
option(BUILD_APPS "Build applications" ON)

# Пути для установки
if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
elseif(APPLE)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
else()
    set(CMAKE_INSTALL_PREFIX "C:/Program Files/EmployeeManagementSystem" CACHE PATH "Installation directory")
endif()

# Поддиректории
add_subdirectory(core)

if(BUILD_APPS)
    add_subdirectory(apps)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Основные цели
add_custom_target(build-all DEPENDS core apps)

if(BUILD_TESTING)
    add_custom_target(test-all DEPENDS unit-tests e2e-tests)
    
    # Запуск тестов после сборки (опционально)
    add_custom_target(run-tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS unit-tests e2e-tests
        COMMENT "Running all tests..."
    )
endif()

# Установка
install(DIRECTORY include/ DESTINATION include)
install(TARGETS employee_core EXPORT EmployeeManagementSystemTargets)

# CPack настройка
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VENDOR "EmployeeManagementSystem")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
include(CPack)



cmake /Users/olga_zalutskaya/Documents/oc/lab 1/CMakeLists.txt 