cmake_minimum_required(VERSION 3.10)
project(ThreadLab)

set(CMAKE_CXX_STANDARD 98)

add_executable(thread_lab main.cpp)
target_link_libraries(thread_lab pthread)