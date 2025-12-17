#pragma once
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "employee_types.h"
#include <fstream>
#include <vector>
#include <mutex>
#include <string>

namespace EmployeeSystem {

    class FileManager {
    private:
        std::string filename_;
        std::fstream file_;
        std::mutex file_mutex_;
        
    public:
        explicit FileManager(const std::string& filename);
        bool open();
        std::vector<Employee> read_all();
        bool write_all(const std::vector<Employee>& employees);
        Employee* find_employee(std::vector<Employee>& employees, int32_t id);
        void close();
        ~FileManager();
    };

} 

#endif 