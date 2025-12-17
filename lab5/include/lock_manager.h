#pragma once
#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include "employee_types.h"
#include <map>
#include <set>
#include <mutex>
#include <condition_variable>

namespace EmployeeSystem {

    class LockManager {
    private:
        std::mutex mutex_;
        std::map<int32_t, std::set<int32_t>> read_locks_;
        std::map<int32_t, int32_t> write_locks_;
        std::condition_variable cv_;
        
    public:
        bool acquire_read_lock(int32_t employee_id, int32_t client_id);
        bool acquire_write_lock(int32_t employee_id, int32_t client_id);
        void release_read_lock(int32_t employee_id, int32_t client_id);
        void release_write_lock(int32_t employee_id, int32_t client_id);
        void release_all_locks(int32_t client_id);
    };

} 

#endif 