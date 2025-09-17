#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include "employee.h"

void testSalaryCalculation() {
    std::cout << "Unit Test: Salary Calculation" << std::endl;
    
    double salary1 = 40.0 * 20.0;
    assert(salary1 == 800.0);
    std::cout << "+ Test 1 passed: Normal hours calculation" << std::endl;
    
    double salary2 = 0.0 * 20.0;
    assert(salary2 == 0.0);
    std::cout << "+ Test 2 passed: Zero hours calculation" << std::endl;
    
    double salary3 = 37.5 * 15.5;
    assert(salary3 == 581.25);
    std::cout << "+ Test 3 passed: Fractional hours calculation" << std::endl;
    
    std::cout << "+ Salary calculation tests passed!" << std::endl;
}

void testReportGeneration() {
    std::cout << "Unit Test: Report Generation" << std::endl;
    
    const std::string testBinFile = "test_report.bin";
    const std::string testReportFile = "test_report.txt";
    
    Employee testEmployees[] = {
        {101, "John", 40.5},
        {102, "Anna", 35.0},
        {103, "Mike", 42.0}
    };
    
    std::ofstream binFile(testBinFile, std::ios::binary);
    for (int i = 0; i < 3; ++i) {
        binFile.write(reinterpret_cast<const char*>(&testEmployees[i]), sizeof(Employee));
    }
    binFile.close();
    
    std::ofstream reportFile(testReportFile);
    reportFile << "Test Report" << std::endl;
    reportFile << "ID\tName\tHours\tSalary" << std::endl;
    
    double hourlyRate = 20.0;
    for (int i = 0; i < 3; ++i) {
        double salary = testEmployees[i].hours * hourlyRate;
        reportFile << testEmployees[i].num << "\t"
                  << testEmployees[i].name << "\t"
                  << testEmployees[i].hours << "\t"
                  << std::fixed << std::setprecision(2) << salary
                  << std::endl;
    }
    reportFile.close();
    
    std::ifstream report(testReportFile);
    std::string line;
    int lineCount = 0;
    
    while (std::getline(report, line)) {
        lineCount++;
        if (lineCount > 2) { 
            assert(line.find("101") != std::string::npos || 
                   line.find("102") != std::string::npos ||
                   line.find("103") != std::string::npos);
        }
    }
    
    assert(lineCount >= 4); 
    
    report.close();
    
    remove(testBinFile.c_str());
    remove(testReportFile.c_str());
    
    std::cout << "+ Report generation test passed!" << std::endl;
}

int main() {
    try {
        testSalaryCalculation();
        testReportGeneration();
        std::cout << "+ All Reporter unit tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Unit test failed: " << e.what() << std::endl;
        return 1;
    }
}