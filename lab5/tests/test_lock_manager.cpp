#include "lock_manager.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>

namespace EmployeeSystem {

class LockManagerTest : public ::testing::Test {
protected:
    LockManager lock_manager_;
    
    void SetUp() override {}
    void TearDown() override {
        for (int i = 1; i <= 10; ++i) {
            lock_manager_.release_all_locks(i);
        }
    }
};

TEST_F(LockManagerTest, AcquireReadLockSuccess) {
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 1));
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 2));
}

TEST_F(LockManagerTest, AcquireReadLockFailsWithWriteLock) {
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 1));
    EXPECT_FALSE(lock_manager_.acquire_read_lock(1, 2)); 
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 1)); 
}

TEST_F(LockManagerTest, AcquireWriteLockSuccess) {
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 1));
}

TEST_F(LockManagerTest, AcquireWriteLockFailsWithActiveReadLocks) {
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 1));
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 2));
    EXPECT_FALSE(lock_manager_.acquire_write_lock(1, 3)); 
}

TEST_F(LockManagerTest, AcquireWriteLockFailsWithOtherWriteLock) {
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 1));
    EXPECT_FALSE(lock_manager_.acquire_write_lock(1, 2)); 
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 1));
}

TEST_F(LockManagerTest, ReleaseReadLock) {
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 1));
    lock_manager_.release_read_lock(1, 1);
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 2)); 
}

TEST_F(LockManagerTest, ReleaseAllLocks) {
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 1));
    EXPECT_TRUE(lock_manager_.acquire_read_lock(2, 1));
    EXPECT_TRUE(lock_manager_.acquire_write_lock(3, 1));
    
    lock_manager_.release_all_locks(1);
    
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 2));
    EXPECT_TRUE(lock_manager_.acquire_write_lock(2, 2));
    EXPECT_TRUE(lock_manager_.acquire_write_lock(3, 2));
}

TEST_F(LockManagerTest, MultipleResources) {
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 1));
    EXPECT_TRUE(lock_manager_.acquire_write_lock(2, 2));
    EXPECT_TRUE(lock_manager_.acquire_read_lock(3, 3));
    
    EXPECT_FALSE(lock_manager_.acquire_read_lock(1, 2));
    EXPECT_FALSE(lock_manager_.acquire_write_lock(2, 1));
    EXPECT_TRUE(lock_manager_.acquire_read_lock(3, 4));
}

TEST_F(LockManagerTest, UpgradeReadToWriteSameClient) {
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 1));
    EXPECT_TRUE(lock_manager_.acquire_write_lock(1, 1)); 
}

TEST_F(LockManagerTest, UpgradeReadToWriteMultipleReaders) {
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 1));
    EXPECT_TRUE(lock_manager_.acquire_read_lock(1, 2));
    EXPECT_FALSE(lock_manager_.acquire_write_lock(1, 1));
}

TEST_F(LockManagerTest, ConcurrentAccessThreadSafety) {
    constexpr int NUM_THREADS = 10;
    constexpr int NUM_OPERATIONS = 100;
    constexpr int EMPLOYEE_ID = 1;
    
    std::vector<std::thread> threads;
    std::atomic<int> successful_operations{0};
    std::atomic<int> failed_operations{0};
    
    for (int thread_id = 0; thread_id < NUM_THREADS; ++thread_id) {
        threads.emplace_back([this, thread_id, &successful_operations, &failed_operations]() {
            int client_id = thread_id + 1;
            
            for (int i = 0; i < NUM_OPERATIONS; ++i) {
                if (i % 4 == 0) {
                    if (lock_manager_.acquire_read_lock(EMPLOYEE_ID, client_id)) {
                        successful_operations++;
                        lock_manager_.release_read_lock(EMPLOYEE_ID, client_id);
                    } else {
                        failed_operations++;
                    }
                } else if (i % 4 == 1) {
                    if (lock_manager_.acquire_write_lock(EMPLOYEE_ID, client_id)) {
                        successful_operations++;
                        lock_manager_.release_write_lock(EMPLOYEE_ID, client_id);
                    } else {
                        failed_operations++;
                    }
                } else if (i % 4 == 2) {
                    lock_manager_.release_all_locks(client_id);
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_GE(successful_operations + failed_operations, NUM_THREADS * NUM_OPERATIONS / 2);
    
    for (int i = 1; i <= NUM_THREADS; ++i) {
        lock_manager_.release_all_locks(i);
    }
    
    EXPECT_TRUE(lock_manager_.acquire_write_lock(EMPLOYEE_ID, NUM_THREADS + 1));
}

} 