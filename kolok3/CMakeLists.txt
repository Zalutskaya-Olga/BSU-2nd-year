cmake_minimum_required(VERSION 3.10)
project(Kolok3Project)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

add_executable(kolok3_demo 
    main.cpp 
    kolok3.cpp
)

find_package(GTest QUIET)
if(GTEST_FOUND)
    message(STATUS "GoogleTest found - building tests")
    
    add_executable(kolok3_tests
        test_kolok3.cpp
        kolok3.cpp
    )
    
    target_link_libraries(kolok3_tests GTest::GTest GTest::Main)
    
    enable_testing()
    add_test(NAME Kolok3Tests COMMAND kolok3_tests)
else()
    message(WARNING "GoogleTest not found - skipping tests")
endif()