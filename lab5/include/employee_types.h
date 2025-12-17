#pragma once
#ifndef EMPLOYEE_TYPES_H
#define EMPLOYEE_TYPES_H

#include <cstdint>
#include <string>
#include <cstring>

namespace EmployeeSystem {
    
    constexpr size_t MAX_CLIENTS = 100;
    constexpr size_t BUFFER_SIZE = 1024;
    constexpr char SERVER_FIFO[] = "/tmp/employee_server_fifo";
    constexpr char CLIENT_FIFO_TEMPLATE[] = "/tmp/employee_client_%d_fifo";

    #pragma pack(push, 1)
    struct Employee {
        int32_t id;
        char name[10];
        double hours;
        
        Employee() : id(0), hours(0.0) {
            memset(name, 0, sizeof(name));
        }
        
        Employee(int32_t id, const std::string& name_str, double hours_val) 
            : id(id), hours(hours_val) {
            memset(name, 0, sizeof(name));
            strncpy(name, name_str.c_str(), sizeof(name) - 1);
        }
        
        bool operator==(const Employee& other) const {
            return id == other.id && 
                   strncmp(name, other.name, sizeof(name)) == 0 &&
                   hours == other.hours;
        }
    };

    enum class OperationType : uint8_t {
        READ = 'R',
        WRITE = 'W',
        UNLOCK = 'U',
        EXIT = 'X'
    };

    enum class ResponseStatus : uint8_t {
        SUCCESS = 'S',
        ERROR = 'E',
        LOCKED = 'L',
        NOT_FOUND = 'N'
    };

    struct Request {
        int32_t client_id;
        int32_t employee_id;
        OperationType operation;
        Employee employee;
        uint64_t timestamp;
        
        Request() : client_id(0), employee_id(0), 
                   operation(OperationType::READ), timestamp(0) {}
    };

    struct Response {
        int32_t employee_id;
        ResponseStatus status;
        Employee employee;
        uint64_t timestamp;
        
        Response() : employee_id(0), status(ResponseStatus::ERROR), timestamp(0) {}
    };
    #pragma pack(pop)

} 

#endif 