cmake_minimum_required(VERSION 3.10)
project(Kolok1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kolok1 kolok1.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(kolok1 PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(kolok1_test PRIVATE -Wall -Wextra -pedantic)
endif()