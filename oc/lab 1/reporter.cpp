#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include "employee.h"

struct ReportEntry {
    Employee emp;
    double salary;
};

double calculateSalary(double hours, double hourlyRate)
{
    return hours * hourlyRate;
}

void generateReport(const std::string& binFile, const std::string& reportFile, double hourlyRate)
{
    std::ifstream bin(binFile.c_str(), std::ios::binary);
    if (!bin.is_open())
    {
        throw std::runtime_error("Failed to open binary file: " + binFile);
    }

    std::vector<ReportEntry> entries;
    Employee emp;
    while (bin.read(reinterpret_cast<char*>(&emp), sizeof(Employee)))
    {
        ReportEntry e;
        e.emp = emp;
        e.salary = calculateSalary(emp.hours, hourlyRate);
        entries.push_back(e);
    }
    bin.close();

    std::ifstream testFile(reportFile.c_str());
    if (testFile.is_open())
    {
        testFile.close();
        throw std::runtime_error("Report file already exists: " + reportFile);
    }

    std::ofstream out(reportFile.c_str());
    if (!out.is_open())
    {
        throw std::runtime_error("Failed to open report file: " + reportFile);
    }

    out << "Report for file \"" << binFile << "\"" << std::endl;
    out << "ID\tName\tHours\tSalary" << std::endl;

    for (size_t i = 0; i < entries.size(); ++i)
    {
        out << entries[i].emp.num << "\t"
            << entries[i].emp.name << "\t"
            << entries[i].emp.hours << "\t"
            << std::fixed << std::setprecision(2) << entries[i].salary
            << std::endl;
    }

    out.close();
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            throw std::invalid_argument("Usage: Reporter <binary_file> <report_file> <hourly_rate>");
        }

        const std::string binFile = argv[1];
        const std::string reportFile = argv[2];
        const double hourlyRate = std::atof(argv[3]);

        if (hourlyRate <= 0)
        {
            throw std::invalid_argument("Hourly rate must be positive");
        }

        generateReport(binFile, reportFile, hourlyRate);
        
        std::cout << "Report successfully generated: " << reportFile << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}