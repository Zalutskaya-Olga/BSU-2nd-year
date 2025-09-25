//cd "/Users/olga_zalutskaya/Documents/oc/lab 1/"
//./creator employees.dat 3

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <limits>
#include <cctype>
#include <cstdlib>
#include "employee.h"

class EmployeeDataCreator
{
public:
    static void createEmployeeFile(const std::string& filename, int recordCount)
    {
        if (recordCount <= 0)
        {
            throw std::invalid_argument("Record count must be positive");
        }

        std::ofstream file(filename.c_str(), std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to create file: " + filename);
        }

        std::cout << "Enter data for " << recordCount << " employees:" << std::endl;
        std::cout << "Type 'exit' at any time to quit" << std::endl;

        for (int i = 0; i < recordCount; ++i)
        {
            std::cout << "\n--- Employee " << (i + 1) << " ---" << std::endl;
            
            Employee emp = createEmployee();
            file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
            
            if (!file.good())
            {
                throw std::runtime_error("Failed to write employee data to file");
            }
            
            std::cout << "✓ Employee " << (i + 1) << " data saved." << std::endl;
        }

        file.close();
        std::cout << "\n✓ File " << filename << " successfully created with " 
                  << recordCount << " records." << std::endl;
    }

private:
    static bool shouldExit(const std::string& input)
    {
        return input == "exit" || input == "quit";
    }

    static int safeInputInt(const std::string& prompt)
    {
        std::string input;
        while (true)
        {
            std::cout << prompt << " (or 'exit' to quit): ";
            std::getline(std::cin, input);
            
            if (shouldExit(input))
            {
                throw std::runtime_error("User requested exit");
            }
            
            if (input.empty())
            {
                std::cout << "Error! Please enter a number or 'exit'." << std::endl;
                continue;
            }
            
            bool valid = true;
            for (std::size_t i = 0; i < input.length(); ++i)
            {
                if (i == 0 && input[i] == '-') continue;
                if (!std::isdigit(input[i]))
                {
                    valid = false;
                    break;
                }
            }
            
            if (!valid)
            {
                std::cout << "Error! Please enter a valid integer or 'exit'." << std::endl;
                continue;
            }
            
            return std::atoi(input.c_str());
        }
    }

    static double safeInputDouble(const std::string& prompt)
    {
        std::string input;
        while (true)
        {
            std::cout << prompt << " (or 'exit' to quit): ";
            std::getline(std::cin, input);
            
            if (shouldExit(input))
            {
                throw std::runtime_error("User requested exit");
            }
            
            if (input.empty())
            {
                std::cout << "Error! Please enter a number or 'exit'." << std::endl;
                continue;
            }
            
            bool valid = true;
            bool has_dot = false;
            for (std::size_t i = 0; i < input.length(); ++i)
            {
                if (i == 0 && input[i] == '-') continue;
                if (input[i] == '.' && !has_dot)
                {
                    has_dot = true;
                    continue;
                }
                if (!std::isdigit(input[i]))
                {
                    valid = false;
                    break;
                }
            }
            
            if (!valid)
            {
                std::cout << "Error! Please enter a valid number or 'exit'." << std::endl;
                continue;
            }
            
            return std::atof(input.c_str());
        }
    }

    static std::string safeInputName()
    {
        std::string input;
        while (true)
        {
            std::cout << "Name (max " << (EMPLOYEE_NAME_SIZE - 1) 
                      << " characters, letters only, or 'exit' to quit): ";
            std::getline(std::cin, input);
            
            if (shouldExit(input))
            {
                throw std::runtime_error("User requested exit");
            }
            
            if (input.empty())
            {
                std::cout << "Error! Name cannot be empty." << std::endl;
                continue;
            }
            
            bool valid = true;
            for (std::size_t i = 0; i < input.length(); ++i)
            {
                if (!std::isalpha(static_cast<unsigned char>(input[i])))
                {
                    valid = false;
                    break;
                }
            }
            
            if (!valid)
            {
                std::cout << "Error! Name must contain only letters." << std::endl;
                continue;
            }
            
            if (input.length() >= static_cast<std::size_t>(EMPLOYEE_NAME_SIZE))
            {
                std::cout << "Error! Name too long. Maximum " 
                          << (EMPLOYEE_NAME_SIZE - 1) << " characters." << std::endl;
                continue;
            }
            
            return input;
        }
    }

    static Employee createEmployee()
    {
        Employee emp;
        emp.num = 0;
        std::memset(emp.name, 0, EMPLOYEE_NAME_SIZE);
        emp.hours = 0.0;
        
        emp.num = safeInputInt("ID number: ");
        
        std::string name = safeInputName();
        std::strncpy(emp.name, name.c_str(), EMPLOYEE_NAME_SIZE - 1);
        emp.name[EMPLOYEE_NAME_SIZE - 1] = '\0';
        
        emp.hours = safeInputDouble("Hours worked: ");
        
        return emp;
    }
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            throw std::invalid_argument("Usage: Creator <filename> <record_count>");
        }
        
        const std::string filename(argv[1]);
        const int recordCount = std::atoi(argv[2]);
        
        if (recordCount <= 0)
        {
            throw std::invalid_argument("Record count must be positive");
        }
        
        EmployeeDataCreator::createEmployeeFile(filename, recordCount);
        
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}


