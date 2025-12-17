cmake_minimum_required(VERSION 3.10)
project(EmployeeSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(server 
    src/server.cpp
    src/file_manager.cpp
    src/lock_manager.cpp
    src/fifo_manager.cpp
    src/logger.cpp
)

add_executable(client 
    src/client.cpp
)

if(APPLE OR UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(server Threads::Threads)
    target_link_libraries(client Threads::Threads)
endif()