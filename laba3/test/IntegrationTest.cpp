#include <gtest/gtest.h>
#include "ArrayManager.h"
#include "ThreadManager.h"
#include "Constants.h"

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(IntegrationTest, ArrayManagerWithThreadManager) {
    ArrayManager arrayManager(20);
    ThreadManager threadManager;
    
    EXPECT_TRUE(threadManager.initialize(5, &arrayManager));
    EXPECT_EQ(threadManager.getActiveThreadCount(), 5);
    
    for (int i = 0; i < arrayManager.getSize(); ++i) {
        EXPECT_EQ(arrayManager.getElement(i), 0);
    }
}

TEST_F(IntegrationTest, ThreadTerminationFlow) {
    ArrayManager arrayManager(15);
    ThreadManager threadManager;
    
    ASSERT_TRUE(threadManager.initialize(4, &arrayManager));
    
    threadManager.startAllThreads();
    
    threadManager.waitForAllSuspensions();
    
    EXPECT_TRUE(threadManager.terminateThread(2));
    threadManager.waitForThreadTermination(2);
    
    EXPECT_EQ(threadManager.getActiveThreadCount(), 3);
    EXPECT_FALSE(threadManager.isThreadActive(2));
    
    threadManager.resumeAllThreads();
}
