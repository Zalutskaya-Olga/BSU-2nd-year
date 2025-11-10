cmake_minimum_required(VERSION 3.10)
project(ThreadLab)

set(CMAKE_CXX_STANDARD 11)


find_package(GTest REQUIRED)
find_package(Threads REQUIRED)


add_executable(thread_lab 
    main.cpp 
    ThreadManager.cpp
    DataManager.cpp
    DataProcessor.cpp
    ThreadController.cpp
)
target_link_libraries(thread_lab pthread)


add_executable(thread_lab_test 
    test_main.cpp 
    ThreadManager.cpp
    DataManager.cpp
    DataProcessor.cpp
    ThreadController.cpp
)
target_link_libraries(thread_lab_test 
    PRIVATE 
    GTest::gtest_main 
    pthread
)


include(GoogleTest)
gtest_discover_tests(thread_lab_test)
