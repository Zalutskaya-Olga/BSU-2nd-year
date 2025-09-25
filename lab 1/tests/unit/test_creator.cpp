#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <stdexcept>
#include "employee.h"

// Мокируем ввод для тестирования
class MockEmployeeDataCreator {
public:
    static void createEmployeeFileWithMockData(const std::string& filename, 
                                             const std::vector<Employee>& employees) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create file");
        }
        
        for (const auto& emp : employees) {
            file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
        }
        file.close();
    }
};

class CreatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testFile = "test_employees.dat";
    }
    
    void TearDown() override {
        remove(testFile.c_str());
    }
    
    std::string testFile;
};

TEST_F(CreatorTest, CreateValidEmployeeFile) {
    std::vector<Employee> testEmployees = {
        {1, "John", 40.5},
        {2, "Alice", 35.0},
        {3, "Bob", 42.0}
    };
    
    MockEmployeeDataCreator::createEmployeeFileWithMockData(testFile, testEmployees);
    
    // Проверяем, что файл создан
    std::ifstream file(testFile, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    
    // Читаем и проверяем данные
    Employee emp;
    for (size_t i = 0; i < testEmployees.size(); ++i) {
        file.read(reinterpret_cast<char*>(&emp), sizeof(Employee));
        EXPECT_TRUE(emp.isValid());
        EXPECT_EQ(emp.num, testEmployees[i].num);
        EXPECT_STREQ(emp.name, testEmployees[i].name);
        EXPECT_DOUBLE_EQ(emp.hours, testEmployees[i].hours);
    }
    
    file.close();
}

TEST_F(CreatorTest, FileCreationFailure) {
    // Попытка создать файл в несуществующей директории
    EXPECT_THROW(MockEmployeeDataCreator::createEmployeeFileWithMockData(
        "/invalid/path/test.dat", {{1, "Test", 10.0}}), 
        std::runtime_error);
}

TEST_F(CreatorTest, EmployeeValidation) {
    Employee validEmp = {1, "Valid", 40.0};
    Employee invalidEmp1 = {0, "Invalid", 40.0}; // ID = 0
    Employee invalidEmp2 = {1, "", 40.0}; // Пустое имя
    Employee invalidEmp3 = {1, "Valid", -5.0}; // Отрицательные часы
    
    EXPECT_TRUE(validEmp.isValid());
    EXPECT_FALSE(invalidEmp1.isValid());
    EXPECT_FALSE(invalidEmp2.isValid());
    EXPECT_FALSE(invalidEmp3.isValid());
}