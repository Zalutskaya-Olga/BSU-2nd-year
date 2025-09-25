#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>
#include <string>
#include <cstring>

const int EMPLOYEE_NAME_SIZE = 10;

struct Employee
{
    int num;
    char name[EMPLOYEE_NAME_SIZE];
    double hours;
    
    Employee();
    Employee(int num, const std::string& name, double hours);
    
    bool operator==(const Employee& other) const;
    bool operator!=(const Employee& other) const;
    bool isValid() const;
    
    friend std::ostream& operator<<(std::ostream& os, const Employee& emp);
    friend std::istream& operator>>(std::istream& is, Employee& emp);
};

#endif