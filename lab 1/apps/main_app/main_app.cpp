#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include "/Users/olga_zalutskaya/Documents/oc/lab1/core/employee.h" 
#include "/Users/olga_zalutskaya/Documents/oc/lab1/core/employee.cpp"  // Исправленный путь

// Временная заглушка для FileUtils, если её нет
namespace FileUtils {
    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
    
    void validateFileDoesNotExist(const std::string& filename) {
        if (fileExists(filename)) {
            throw std::runtime_error("File already exists: " + filename);
        }
    }
}

class EmployeeManager {
public:
    void run() {
        try {
            std::string binFile = getInput("Enter binary filename: ");
            int recordCount = std::stoi(getInput("Enter record count: "));
            
            FileUtils::validateFileDoesNotExist(binFile);
            
            // Запуск creator
            std::string creatorCmd = "./creator " + binFile + " " + std::to_string(recordCount);
            executeCommand(creatorCmd, "Creator");
            
            // Показать содержимое бинарного файла
            printBinaryFile(binFile);
            
            // Запуск reporter
            std::string reportFile = getInput("Enter report filename: ");
            double hourlyRate = std::stod(getInput("Enter hourly rate: "));
            
            FileUtils::validateFileDoesNotExist(reportFile);
            
            std::string reporterCmd = "./reporter " + binFile + " " + reportFile + " " + std::to_string(hourlyRate);
            executeCommand(reporterCmd, "Reporter");
            
            // Показать отчет
            printReport(reportFile);
            
            std::cout << "Workflow completed successfully!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

private:
    std::string getInput(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }
    
    void executeCommand(const std::string& command, const std::string& component) {
        int result = std::system(command.c_str());
        if (result != 0) {
            throw std::runtime_error(component + " execution failed");
        }
    }
    
    void printBinaryFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
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
        std::ifstream file(reportFile);
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
};

int main() {
    EmployeeManager manager;
    manager.run();
    return 0;
}