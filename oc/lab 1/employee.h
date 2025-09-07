#ifndef EMPLOYEE_H
#define EMPLOYEE_H

const int EMPLOYEE_NAME_SIZE = 10;

struct Employee
{
    int num;        // employee ID
    char name[EMPLOYEE_NAME_SIZE];  // employee name
    double hours;   // hours worked
};

#endif // EMPLOYEE_H
