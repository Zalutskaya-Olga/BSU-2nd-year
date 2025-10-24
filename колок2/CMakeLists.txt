cmake_minimum_required(VERSION 3.10)
project(Kolok2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(kolok2_main main.cpp kolok2.cpp)
enable_testing()
find_package(GTest REQUIRED)

add_executable(kolok2_test test_kolok2.cpp kolok2.cpp)
target_link_libraries(kolok2_test 
    GTest::gtest 
    GTest::gtest_main 
    pthread
)
target_compile_options(kolok2_test PRIVATE -pthread)
