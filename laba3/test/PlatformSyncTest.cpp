// PlatformSyncTest.cpp
#include <gtest/gtest.h>
#include "PlatformSync.h"
#include "Constants.h"
#include <thread>
#include <atomic>

class PlatformSyncTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(PlatformSyncTest, CreateDestroyEvent) {
    SyncEvent event;
    EXPECT_NO_THROW(event = PlatformSync::createEvent(false));
    EXPECT_NO_THROW(PlatformSync::destroyEvent(&event));
}

TEST_F(PlatformSyncTest, CreateDestroyMutex) {
    SyncMutex mutex;
    EXPECT_NO_THROW(mutex = PlatformSync::createMutex());
    EXPECT_NO_THROW(PlatformSync::destroyMutex(&mutex));
}

TEST_F(PlatformSyncTest, MutexLockUnlock) {
    SyncMutex mutex = PlatformSync::createMutex();
    
    EXPECT_NO_THROW(PlatformSync::enterCriticalSection(&mutex));
    EXPECT_NO_THROW(PlatformSync::leaveCriticalSection(&mutex));
    
    PlatformSync::destroyMutex(&mutex);
}

TEST_F(PlatformSyncTest, EventSetReset) {
    SyncEvent event = PlatformSync::createEvent(true); // Manual reset
    
    EXPECT_NO_THROW(PlatformSync::setEvent(&event));
    EXPECT_NO_THROW(PlatformSync::resetEvent(&event));
    
    PlatformSync::destroyEvent(&event);
}

TEST_F(PlatformSyncTest, EventWaitWithTimeout) {
    SyncEvent event = PlatformSync::createEvent(false);
    
    // Should timeout immediately since event is not signaled
    bool result = PlatformSync::waitForEvent(&event, 10); // 10ms timeout
    EXPECT_FALSE(result);
    
    PlatformSync::destroyEvent(&event);
}

TEST_F(PlatformSyncTest, ThreadCreation) {
    auto threadFunc = [](void* param) -> void* {
        int* value = static_cast<int*>(param);
        *value = 42;
        return nullptr;
    };
    
    int value = 0;
    SyncThread thread;
    EXPECT_NO_THROW(thread = PlatformSync::createThread(threadFunc, &value));
    EXPECT_NO_THROW(PlatformSync::waitForThread(thread));
    
    EXPECT_EQ(value, 42);
}

TEST_F(PlatformSyncTest, SleepFunction) {
    auto start = std::chrono::steady_clock::now();
    PlatformSync::sleep(50); // Sleep for 50ms
    auto end = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(duration.count(), 45); // Allow some tolerance
}