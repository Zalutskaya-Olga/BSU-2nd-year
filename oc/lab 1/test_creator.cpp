#include <iostream>
#include <sstream>
#include <cassert>
#include <fstream>
#include "employee.h"

class TestEmployeeDataCreator {
public:
    static Employee createTestEmployee(int id, const std::string& name, double hours) {
        Employee emp;
        emp.num = id;
        std::strncpy(emp.name, name.c_str(), EMPLOYEE_NAME_SIZE - 1);
        emp.name[EMPLOYEE_NAME_SIZE - 1] = '\0';
        emp.hours = hours;
        return emp;
    }

    static void createTestEmployeeFile(const std::string& filename, const Employee* employees, int count) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create test file");
        }

        for (int i = 0; i < count; ++i) {
            file.write(reinterpret_cast<const char*>(&employees[i]), sizeof(Employee));
        }

        file.close();
    }
};

void testFileCreation() {
    std::cout << "Unit Test: File Creation " << std::endl;
    
    const std::string testFile = "test_creation.bin";
    
    Employee testEmployees[] = {
        TestEmployeeDataCreator::createTestEmployee(101, "John", 40.5),
        TestEmployeeDataCreator::createTestEmployee(102, "Anna", 35.0),
        TestEmployeeDataCreator::createTestEmployee(103, "Mike", 42.0)
    };
    
    TestEmployeeDataCreator::createTestEmployeeFile(testFile, testEmployees, 3);
    
    std::ifstream file(testFile, std::ios::binary);
    assert(file.is_open());
    
    Employee emp;
    int count = 0;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        assert(emp == testEmployees[count]);
        assert(emp.isValid());
        count++;
    }
    
    assert(count == 3);
    file.close();
    
    remove(testFile.c_str());
    
    std::cout << "+ File creation test passed!" << std::endl;
}

void testDataValidation() {
    std::cout << "Unit Test: Data Validation" << std::endl;
    
    Employee validEmp = TestEmployeeDataCreator::createTestEmployee(100, "Valid", 40.0);
    assert(validEmp.isValid());
    
    Employee invalidEmp1 = TestEmployeeDataCreator::createTestEmployee(-1, "Invalid", 40.0);
    assert(!invalidEmp1.isValid());
    
    Employee invalidEmp2 = TestEmployeeDataCreator::createTestEmployee(100, "Invalid", -5.0);
    assert(!invalidEmp2.isValid());
    
    std::cout << "+ Data validation test passed!" << std::endl;
}

int main() {
    try {
        testFileCreation();
        testDataValidation();
        std::cout << "+ All Creator unit tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Unit test failed: " << e.what() << std::endl;
        return 1;
    }
}