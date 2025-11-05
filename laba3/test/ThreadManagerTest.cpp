// ThreadManagerTest.cpp
#include <gtest/gtest.h>
#include "ThreadManager.h"
#include "ArrayManager.h"
#include "Constants.h"

class ThreadManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        arrayManager = new ArrayManager(10);
    }

    void TearDown() override {
        delete arrayManager;
    }
    
    ArrayManager* arrayManager;
};

TEST_F(ThreadManagerTest, ConstructorDestructor) {
    EXPECT_NO_THROW({
        ThreadManager manager;
    });
}

TEST_F(ThreadManagerTest, InitializeValidParameters) {
    ThreadManager manager;
    
    EXPECT_TRUE(manager.initialize(3, arrayManager));
    EXPECT_EQ(manager.getActiveThreadCount(), 3);
}

TEST_F(ThreadManagerTest, InitializeInvalidParameters) {
    ThreadManager manager;
    
    // Invalid thread count
    EXPECT_FALSE(manager.initialize(0, arrayManager));
    EXPECT_FALSE(manager.initialize(Constants::MAX_THREAD_COUNT + 1, arrayManager));
    
    // Null array manager
    EXPECT_FALSE(manager.initialize(3, nullptr));
}

TEST_F(ThreadManagerTest, ThreadActiveStatus) {
    ThreadManager manager;
    ASSERT_TRUE(manager.initialize(3, arrayManager));
    
    EXPECT_TRUE(manager.isThreadActive(1));
    EXPECT_TRUE(manager.isThreadActive(2));
    EXPECT_TRUE(manager.isThreadActive(3));
    EXPECT_FALSE(manager.isThreadActive(0)); // Invalid ID
    EXPECT_FALSE(manager.isThreadActive(4)); // Invalid ID
}

TEST_F(ThreadManagerTest, Cleanup) {
    ThreadManager manager;
    ASSERT_TRUE(manager.initialize(2, arrayManager));
    
    EXPECT_NO_THROW(manager.cleanup());
    EXPECT_EQ(manager.getActiveThreadCount(), 0);
}