cmake_minimum_required(VERSION 3.10)
project(Lab4_Sync)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(receiver receiver.cpp)
add_executable(sender sender.cpp)

target_compile_options(receiver PRIVATE -Wall -Wextra)
target_compile_options(sender PRIVATE -Wall -Wextra)

target_link_libraries(receiver pthread)
target_link_libraries(sender pthread)
