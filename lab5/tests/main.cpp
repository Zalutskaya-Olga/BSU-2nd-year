#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "==========================================\n";
    std::cout << "   Employee System Test Suite\n";
    std::cout << "==========================================\n";
    
    const char* test_dir = "/tmp/employee_system_tests";
    system(("mkdir -p " + std::string(test_dir)).c_str());
    system(("chmod 777 " + std::string(test_dir)).c_str());
    
    int result = RUN_ALL_TESTS();
    
    system(("rm -rf " + std::string(test_dir)).c_str());
    
    return result;
}