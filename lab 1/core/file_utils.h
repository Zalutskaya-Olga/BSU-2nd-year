#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include "employee.h"

namespace FileUtils {
    bool fileExists(const std::string& filename);
    void validateFileDoesNotExist(const std::string& filename);
    std::vector<Employee> readEmployeesFromFile(const std::string& filename);
    void writeEmployeesToFile(const std::string& filename, const std::vector<Employee>& employees);
}

#endif