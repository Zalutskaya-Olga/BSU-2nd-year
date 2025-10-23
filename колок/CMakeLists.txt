cmake_minimum_required(VERSION 3.10)
project(Kolok1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kolok1 kolok1.cpp)

enable_testing()
find_package(GTest REQUIRED)
add_executable(kolok1_test kolok1_gtest.cpp)
target_link_libraries(kolok1_test GTest::GTest GTest::Main)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(kolok1 PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(kolok1_test PRIVATE -Wall -Wextra -pedantic)
endif()
