#include "employee.h"
#include <stdexcept>

Employee::Employee() : num(0), hours(0.0) {
    std::memset(name, 0, EMPLOYEE_NAME_SIZE);
}

Employee::Employee(int num, const std::string& name, double hours) 
    : num(num), hours(hours) {
    if (name.length() >= EMPLOYEE_NAME_SIZE) {
        throw std::invalid_argument("Name too long");
    }
    std::strncpy(this->name, name.c_str(), EMPLOYEE_NAME_SIZE - 1);
    this->name[EMPLOYEE_NAME_SIZE - 1] = '\0';
}

bool Employee::operator==(const Employee& other) const {
    return num == other.num && 
           std::strcmp(name, other.name) == 0 && 
           hours == other.hours;
}

bool Employee::operator!=(const Employee& other) const {
    return !(*this == other);
}

bool Employee::isValid() const {
    return num > 0 && 
           std::strlen(name) > 0 && 
           hours >= 0;
}

std::ostream& operator<<(std::ostream& os, const Employee& emp) {
    os << emp.num << " " << emp.name << " " << emp.hours;
    return os;
}

std::istream& operator>>(std::istream& is, Employee& emp) {
    std::string name;
    is >> emp.num >> name >> emp.hours;
    
    if (name.length() >= EMPLOYEE_NAME_SIZE) {
        is.setstate(std::ios::failbit);
        return is;
    }
    
    std::strncpy(emp.name, name.c_str(), EMPLOYEE_NAME_SIZE - 1);
    emp.name[EMPLOYEE_NAME_SIZE - 1] = '\0';
    return is;
}