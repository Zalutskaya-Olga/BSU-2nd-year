#pragma once
#ifndef FIFO_MANAGER_H
#define FIFO_MANAGER_H

#include "employee_types.h"
#include <fstream>
#include <memory>
#include <string>

namespace EmployeeSystem {

    class FIFOManager {
    public:
        static bool create_fifo(const std::string& path);
        static bool remove_fifo(const std::string& path);
        static std::unique_ptr<std::fstream> open_fifo(const std::string& path, 
                                                     std::ios_base::openmode mode);
    };

} 

#endif 