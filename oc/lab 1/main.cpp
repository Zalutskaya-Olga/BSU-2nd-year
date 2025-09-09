#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include "employee.h"

void printBinaryFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }

    Employee emp;
    std::cout << "\nBinary file content " << filename << ":\n";
    std::cout << "ID\tName\tHours\n";

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        std::cout << emp.num << "\t" << emp.name << "\t" << emp.hours << std::endl;
    }

    file.close();
}

void printReport(const std::string& reportFile) {
    std::ifstream file(reportFile.c_str());
    if (!file.is_open()) {
        std::cerr << "Cannot open report file: " << reportFile << std::endl;
        return;
    }

    std::cout << "\nReport content " << reportFile << ":\n";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
}

int main() {
    try {
        std::string binFile, reportFile;
        int recordCount;
        double hourlyRate;

        std::cout << "Enter binary filename: ";
        std::cin >> binFile;
        std::cout << "Enter record count: ";
        std::cin >> recordCount;

        std::ifstream testFile(binFile.c_str());
        if (testFile.is_open()) {
            std::cerr << "Error: File already exists: " << binFile << std::endl;
            testFile.close();
            return 1;
        }

        std::ostringstream creatorCmd;
        creatorCmd << "./creator " << binFile << " " << recordCount;
        
        if (system(creatorCmd.str().c_str()) != 0) {
            std::cerr << "Error running Creator" << std::endl;
            return 1;
        }

        printBinaryFile(binFile);

        std::cout << "\nEnter report filename: ";
        std::cin >> reportFile;
        std::cout << "Enter hourly rate: ";
        std::cin >> hourlyRate;

        std::ifstream testReportFile(reportFile.c_str());
        if (testReportFile.is_open()) {
            std::cerr << "Error: Report file already exists: " << reportFile << std::endl;
            testReportFile.close();
            return 1;
        }

        std::ostringstream reporterCmd;
        reporterCmd << "./reporter " << binFile << " " << reportFile << " " << hourlyRate;
        
        if (system(reporterCmd.str().c_str()) != 0) {
            std::cerr << "Error running Reporter" << std::endl;
            return 1;
        }

        printReport(reportFile);

        return 0;
    }
    catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return 1;
    }
}


