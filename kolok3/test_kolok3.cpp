#include "kolok3.h"
#include <gtest/gtest.h>
#include <stdexcept>

class LinkedListTest : public ::testing::Test {
protected:
    void SetUp() override {
        list = new LinkedList();
    }
    
    void TearDown() override {
        delete list;
    }
    
    LinkedList* list;
};

TEST_F(LinkedListTest, EmptyList) {
    EXPECT_TRUE(list->isEmpty());
    
    list->reverseRecursive();
    EXPECT_TRUE(list->isEmpty());
}

TEST_F(LinkedListTest, SingleElementList) {
    list->append(100);
    EXPECT_FALSE(list->isEmpty());
    
    list->reverseRecursive();
    EXPECT_FALSE(list->isEmpty());
}

TEST_F(LinkedListTest, MultipleElementsReversal) {
    list->append(1);
    list->append(2);
    list->append(3);
    
    EXPECT_FALSE(list->isEmpty());
    
    list->reverseRecursive();
    EXPECT_FALSE(list->isEmpty());
}

TEST_F(LinkedListTest, DoubleReversal) {
    list->append(10);
    list->append(20);
    list->append(30);
    
    list->reverseRecursive();
    list->reverseRecursive();
    
    EXPECT_FALSE(list->isEmpty());
}

TEST_F(LinkedListTest, LargeListMemoryManagement) {
    const int size = 50;
    
    for (int i = 0; i < size; ++i) {
        list->append(i);
    }
    
    EXPECT_FALSE(list->isEmpty());
    
    list->reverseRecursive();
    list->reverseRecursive();
    list->reverseRecursive();
    
    EXPECT_FALSE(list->isEmpty());
}

TEST_F(LinkedListTest, ClearFunctionality) {
    list->append(1);
    list->append(2);
    list->append(3);
    
    EXPECT_FALSE(list->isEmpty());
    
    list->clear();
    
    EXPECT_TRUE(list->isEmpty());
    
    list->append(99);
    EXPECT_FALSE(list->isEmpty());
}

TEST_F(LinkedListTest, SequentialOperations) {
    EXPECT_TRUE(list->isEmpty());
    
    list->append(5);
    EXPECT_FALSE(list->isEmpty());
    
    list->reverseRecursive();
    EXPECT_FALSE(list->isEmpty());
    
    list->clear();
    EXPECT_TRUE(list->isEmpty());
    
    list->append(10);
    list->append(20);
    EXPECT_FALSE(list->isEmpty());
}

TEST_F(LinkedListTest, ReversePreservesElements) {
    const int testData[] = {1, 2, 3, 4, 5};
    const int dataSize = 5;
    
    for (int i = 0; i < dataSize; ++i) {
        list->append(testData[i]);
    }
    
    list->reverseRecursive();
    EXPECT_FALSE(list->isEmpty());
    
    list->reverseRecursive();
    EXPECT_FALSE(list->isEmpty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}