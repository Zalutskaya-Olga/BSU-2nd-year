# cmake_minimum_required(VERSION 3.10)
# project(Lab3_Synchronization)

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Include directories
# include_directories(${CMAKE_SOURCE_DIR}/include)

# # Platform-specific settings
# if(UNIX)
#     find_package(Threads REQUIRED)
# endif()

# add_executable(lab3 
#     src/main.cpp 
#     src/ThreadManager.cpp 
#     src/ArrayManager.cpp
#     src/PlatformSync.cpp
# )

# if(UNIX)
#     target_link_libraries(lab3 Threads::Threads)
# endif()

# # Google Test
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9fdabf8a2c134a53bad34e7.zip
# )

# FetchContent_MakeAvailable(googletest)

# # Test executables
# add_executable(
#   array_manager_test
#   test/ArrayManagerTest.cpp
#   src/ArrayManager.cpp
# )
# target_link_libraries(
#   array_manager_test
#   GTest::gtest_main
#   ${CMAKE_THREAD_LIBS_INIT}
# )

# add_executable(
#   platform_sync_test
#   test/PlatformSyncTest.cpp
#   src/PlatformSync.cpp
# )
# target_link_libraries(
#   platform_sync_test
#   GTest::gtest_main
#   ${CMAKE_THREAD_LIBS_INIT}
# )

# add_executable(
#   thread_manager_test
#   test/ThreadManagerTest.cpp
#   src/ThreadManager.cpp
#   src/ArrayManager.cpp
#   src/PlatformSync.cpp
# )
# target_link_libraries(
#   thread_manager_test
#   GTest::gtest_main
#   ${CMAKE_THREAD_LIBS_INIT}
# )

# add_executable(
#   integration_test
#   test/IntegrationTest.cpp
#   src/ThreadManager.cpp
#   src/ArrayManager.cpp
#   src/PlatformSync.cpp
# )
# target_link_libraries(
#   integration_test
#   GTest::gtest_main
#   ${CMAKE_THREAD_LIBS_INIT}
# )

# # Include directories
# target_include_directories(array_manager_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
# target_include_directories(platform_sync_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
# target_include_directories(thread_manager_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
# target_include_directories(integration_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # Add tests
# include(GoogleTest)
# gtest_discover_tests(array_manager_test)
# gtest_discover_tests(platform_sync_test)
# gtest_discover_tests(thread_manager_test)
# gtest_discover_tests(integration_test)



cmake_minimum_required(VERSION 3.10)
project(Lab3_Synchronization)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find packages
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# Main executable
add_executable(lab3 
    src/main.cpp 
    src/ThreadManager.cpp 
    src/ArrayManager.cpp
    src/PlatformSync.cpp
)

target_link_libraries(lab3 Threads::Threads)

# Test executables
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

# Link test executables with GTest
foreach(test_target 
    array_manager_test 
    platform_sync_test 
    thread_manager_test 
    integration_test
)
    target_include_directories(${test_target} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_target} GTest::gtest GTest::gtest_main Threads::Threads)
endforeach()

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(array_manager_test)
gtest_discover_tests(platform_sync_test)
gtest_discover_tests(thread_manager_test)
gtest_discover_tests(integration_test)