#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "employee.h"

double calculateSalary(double hours, double hourlyRate) {
    return hours * hourlyRate;
}

class ReporterTest : public ::testing::Test {
protected:
    void SetUp() override {
        binFile = "test_bin.dat";
        reportFile = "test_report.txt";
        
        // Создаем тестовые данные
        std::vector<Employee> testEmployees = {
            {1, "John", 40.0},
            {2, "Alice", 35.5},
            {3, "Bob", 42.0}
        };
        
        std::ofstream file(binFile, std::ios::binary);
        for (const auto& emp : testEmployees) {
            file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
        }
        file.close();
    }
    
    void TearDown() override {
        remove(binFile.c_str());
        remove(reportFile.c_str());
    }
    
    std::string binFile;
    std::string reportFile;
};

TEST_F(ReporterTest, CalculateSalaryCorrectly) {
    EXPECT_DOUBLE_EQ(calculateSalary(40.0, 10.0), 400.0);
    EXPECT_DOUBLE_EQ(calculateSalary(35.5, 15.0), 532.5);
    EXPECT_DOUBLE_EQ(calculateSalary(0.0, 10.0), 0.0);
}

TEST_F(ReporterTest, GenerateReportWithValidData) {
    double hourlyRate = 10.0;
    
    // Читаем бинарный файл и генерируем отчет
    std::ifstream bin(binFile, std::ios::binary);
    std::ofstream out(reportFile);
    
    ASSERT_TRUE(bin.is_open());
    ASSERT_TRUE(out.is_open());
    
    out << "Report for file \"" << binFile << "\"" << std::endl;
    out << "ID\tName\tHours\tSalary" << std::endl;
    
    Employee emp;
    while (bin.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        double salary = calculateSalary(emp.hours, hourlyRate);
        out << emp.num << "\t"
            << emp.name << "\t"
            << emp.hours << "\t"
            << std::fixed << std::setprecision(2) << salary
            << std::endl;
    }
    
    bin.close();
    out.close();
    
    // Проверяем содержимое отчета
    std::ifstream report(reportFile);
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(report, line)) {
        lines.push_back(line);
    }
    
    ASSERT_GE(lines.size(), 4); // Заголовок + 3 сотрудника
    EXPECT_TRUE(lines[0].find("Report for file") != std::string::npos);
    EXPECT_TRUE(lines[1].find("ID\tName\tHours\tSalary") != std::string::npos);
}

TEST_F(ReporterTest, HandleNonExistentBinaryFile) {
    EXPECT_THROW({
        std::ifstream bin("nonexistent.dat", std::ios::binary);
        if (!bin.is_open()) {
            throw std::runtime_error("File not found");
        }
    }, std::runtime_error);
}
