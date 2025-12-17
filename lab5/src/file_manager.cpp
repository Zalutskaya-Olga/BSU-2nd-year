#include "file_manager.h"
#include "employee_types.h"
#include <algorithm>

namespace EmployeeSystem {

    FileManager::FileManager(const std::string& filename) : filename_(filename) {}
    
    bool FileManager::open() {
        std::lock_guard<std::mutex> lock(file_mutex_);
        file_.open(filename_, std::ios::binary | std::ios::in | std::ios::out);
        if (!file_.is_open()) {
            file_.open(filename_, std::ios::binary | std::ios::out);
            file_.close();
            file_.open(filename_, std::ios::binary | std::ios::in | std::ios::out);
        }
        return file_.is_open();
    }
    
    std::vector<Employee> FileManager::read_all() {
        std::lock_guard<std::mutex> lock(file_mutex_);
        std::vector<Employee> employees;
        
        file_.seekg(0, std::ios::end);
        size_t file_size = file_.tellg();
        file_.seekg(0, std::ios::beg);
        
        size_t num_employees = file_size / sizeof(Employee);
        employees.resize(num_employees);
        
        if (num_employees > 0) {
            file_.read(reinterpret_cast<char*>(employees.data()), 
                      num_employees * sizeof(Employee));
        }
        
        return employees;
    }
    
    bool FileManager::write_all(const std::vector<Employee>& employees) {
        std::lock_guard<std::mutex> lock(file_mutex_);
        
        std::string temp_filename = filename_ + ".tmp";
        std::ofstream temp_file(temp_filename, std::ios::binary | std::ios::trunc);
        
        if (!temp_file.is_open()) {
            return false;
        }
        
        if (!employees.empty()) {
            temp_file.write(reinterpret_cast<const char*>(employees.data()),
                           employees.size() * sizeof(Employee));
        }
        
        temp_file.close();
        
        std::ifstream check_file(temp_filename, std::ios::binary | std::ios::ate);
        if (!check_file.is_open() || check_file.tellg() != employees.size() * sizeof(Employee)) {
            std::remove(temp_filename.c_str());
            return false;
        }
        check_file.close();
        
        if (std::rename(temp_filename.c_str(), filename_.c_str()) != 0) {
            return false;
        }
        
        file_.close();
        file_.open(filename_, std::ios::binary | std::ios::in | std::ios::out);
        
        return file_.is_open();
    }
    
    Employee* FileManager::find_employee(std::vector<Employee>& employees, int32_t id) {
        auto it = std::find_if(employees.begin(), employees.end(),
                             [id](const Employee& emp) { return emp.id == id; });
        return it != employees.end() ? &(*it) : nullptr;
    }
    
    void FileManager::close() {
        std::lock_guard<std::mutex> lock(file_mutex_);
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    FileManager::~FileManager() {
        close();
    }

} 