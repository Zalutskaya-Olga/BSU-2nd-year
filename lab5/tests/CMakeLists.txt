cmake_minimum_required(VERSION 3.10)
project(EmployeeSystemTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MACOSX_RPATH ON)

if(APPLE)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
endif()

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ../include
    ..
)

add_library(employee_system_objects STATIC
    ../src/file_manager.cpp
    ../src/lock_manager.cpp
    ../src/fifo_manager.cpp
    ../src/logger.cpp
)

add_executable(employee_system_tests
    test_employee_types.cpp
    test_file_manager.cpp
    test_lock_manager.cpp
    test_fifo_manager.cpp
    test_integration.cpp
    main.cpp
)

if(APPLE)
    target_link_libraries(employee_system_tests
        employee_system_objects
        gtest
        gtest_main
        gmock
        Threads::Threads
    )
    
    target_link_options(employee_system_tests PRIVATE 
        -Wl,-search_paths_first
        -pthread
    )
else()
    target_link_libraries(employee_system_tests
        employee_system_objects
        ${GTEST_LIBRARIES}
        Threads::Threads
        pthread
    )
endif()