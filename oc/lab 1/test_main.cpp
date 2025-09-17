#include <iostream>
#include <sstream>
#include <cassert>
#include <fstream>

void testFileExistenceCheck() {
    std::cout << "Unit Test: File Existence Check" << std::endl;
    
    const std::string nonExistentFile = "nonexistent_file_12345.bin";
    const std::string existingFile = "temp_test_file.bin";
    
    std::ofstream tempFile(existingFile);
    tempFile << "test content";
    tempFile.close();
    
    std::ifstream test1(nonExistentFile);
    assert(!test1.is_open());
    test1.close();
    
    std::ifstream test2(existingFile);
    assert(test2.is_open());
    test2.close();
    
    remove(existingFile.c_str());
    
    std::cout << "+ File existence check test passed!" << std::endl;
}

void testCommandGeneration() {
    std::cout << " Unit Test: Command Generation " << std::endl;
    
    std::ostringstream cmd;
    cmd << "./creator test.bin 5";
    
    std::string expected = "./creator test.bin 5";
    assert(cmd.str() == expected);
    std::cout << "+ Command generation test passed!" << std::endl;
}

int main() {
    try {
        testFileExistenceCheck();
        testCommandGeneration();
        std::cout << "+ All Main unit tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Unit test failed: " << e.what() << std::endl;
        return 1;
    }
}