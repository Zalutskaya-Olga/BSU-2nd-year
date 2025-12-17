#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

namespace EmployeeSystem {

    class Logger {
    public:
        enum class Level { DEBUG, INFO, WARN, ERROR };
        
        static void log(Level level, const std::string& message);
        static void debug(const std::string& message);
        static void info(const std::string& message);
        static void warn(const std::string& message);
        static void error(const std::string& message);

    private:
        static const char* levelToString(Level level);
    };

} 

#endif 