cmake_minimum_required(VERSION 3.10)
project(Lab3_Synchronization)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

add_executable(lab3 
    src/main.cpp 
    src/ThreadManager.cpp 
    src/ArrayManager.cpp
    src/PlatformSync.cpp
)

target_link_libraries(lab3 Threads::Threads)

add_executable(
    array_manager_test
    test/ArrayManagerTest.cpp
    src/ArrayManager.cpp
)

add_executable(
    platform_sync_test
    test/PlatformSyncTest.cpp
    src/PlatformSync.cpp
)

add_executable(
    thread_manager_test
    test/ThreadManagerTest.cpp
    src/ThreadManager.cpp
    src/ArrayManager.cpp
    src/PlatformSync.cpp
)

add_executable(
    integration_test
    test/IntegrationTest.cpp
    src/ThreadManager.cpp
    src/ArrayManager.cpp
    src/PlatformSync.cpp
)

foreach(test_target 
    array_manager_test 
    platform_sync_test 
    thread_manager_test 
    integration_test
)
    target_include_directories(${test_target} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_target} GTest::gtest GTest::gtest_main Threads::Threads)
endforeach()

include(GoogleTest)
gtest_discover_tests(array_manager_test)
gtest_discover_tests(platform_sync_test)
gtest_discover_tests(thread_manager_test)
gtest_discover_tests(integration_test)
