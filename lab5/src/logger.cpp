#include "logger.h"
#include <iomanip>
#include <chrono>

namespace EmployeeSystem {

    const char* Logger::levelToString(Level level) {
        static const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};
        return level_str[static_cast<int>(level)];
    }

    void Logger::log(Level level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::cout << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] "
                  << "[" << levelToString(level) << "] " 
                  << message << std::endl;
    }

    void Logger::debug(const std::string& message) { log(Level::DEBUG, message); }
    void Logger::info(const std::string& message) { log(Level::INFO, message); }
    void Logger::warn(const std::string& message) { log(Level::WARN, message); }
    void Logger::error(const std::string& message) { log(Level::ERROR, message); }

} 