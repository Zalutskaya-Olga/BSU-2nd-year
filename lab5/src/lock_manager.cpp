#include "lock_manager.h"

namespace EmployeeSystem {

    bool LockManager::acquire_read_lock(int32_t employee_id, int32_t client_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        auto write_it = write_locks_.find(employee_id);
        if (write_it != write_locks_.end() && write_it->second != client_id) {
            return false;
        }
        
        read_locks_[employee_id].insert(client_id);
        return true;
    }
    
    bool LockManager::acquire_write_lock(int32_t employee_id, int32_t client_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        auto read_it = read_locks_.find(employee_id);
        if (read_it != read_locks_.end()) {
            if (read_it->second.size() > 1 || 
                (read_it->second.size() == 1 && !read_it->second.count(client_id))) {
                return false;
            }
        }
        
        auto write_it = write_locks_.find(employee_id);
        if (write_it != write_locks_.end() && write_it->second != client_id) {
            return false;
        }
        
        write_locks_[employee_id] = client_id;
        return true;
    }
    
    void LockManager::release_read_lock(int32_t employee_id, int32_t client_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = read_locks_.find(employee_id);
        if (it != read_locks_.end()) {
            it->second.erase(client_id);
            if (it->second.empty()) {
                read_locks_.erase(it);
            }
        }
        cv_.notify_all();
    }
    
    void LockManager::release_write_lock(int32_t employee_id, int32_t client_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = write_locks_.find(employee_id);
        if (it != write_locks_.end() && it->second == client_id) {
            write_locks_.erase(it);
        }
        cv_.notify_all();
    }
    
    void LockManager::release_all_locks(int32_t client_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto it = read_locks_.begin(); it != read_locks_.end(); ) {
            it->second.erase(client_id);
            if (it->second.empty()) {
                it = read_locks_.erase(it);
            } else {
                ++it;
            }
        }
        
        for (auto it = write_locks_.begin(); it != write_locks_.end(); ) {
            if (it->second == client_id) {
                it = write_locks_.erase(it);
            } else {
                ++it;
            }
        }
        
        cv_.notify_all();
    }

} 