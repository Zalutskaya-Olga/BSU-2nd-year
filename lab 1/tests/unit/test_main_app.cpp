#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "employee.h"

class MainTest : public ::testing::Test {
protected:
    void TearDown() override {
        system("rm -f test_*.dat test_*.txt 2>/dev/null");
    }
};

TEST_F(MainTest, SystemCommandsWork) {
    // Тестируем создание файла через system
    int result = system("touch test_file.txt");
    EXPECT_EQ(result, 0);
    
    // Проверяем, что файл создан
    std::ifstream file("test_file.txt");
    EXPECT_TRUE(file.is_open());
    file.close();
    
    remove("test_file.txt");
}

TEST_F(MainTest, BinaryFileContentPrinting) {
    // Создаем тестовый бинарный файл
    Employee emp = {123, "TestUser", 45.5};
    std::ofstream out("test_print.dat", std::ios::binary);
    out.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    out.close();
    
    // Читаем и проверяем содержимое
    std::ifstream in("test_print.dat", std::ios::binary);
    Employee readEmp;
    in.read(reinterpret_cast<char*>(&readEmp), sizeof(Employee));
    in.close();
    
    EXPECT_EQ(readEmp.num, 123);
    EXPECT_STREQ(readEmp.name, "TestUser");
    EXPECT_DOUBLE_EQ(readEmp.hours, 45.5);
    
    remove("test_print.dat");
}