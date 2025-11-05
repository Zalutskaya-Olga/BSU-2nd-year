// ArrayManagerTest.cpp
#include <gtest/gtest.h>
#include "ArrayManager.h"
#include "Constants.h"

class ArrayManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(ArrayManagerTest, ConstructorValidSize) {
    EXPECT_NO_THROW({
        ArrayManager manager(10);
        EXPECT_EQ(manager.getSize(), 10);
    });
}

TEST_F(ArrayManagerTest, ConstructorInvalidSize) {
    EXPECT_THROW(ArrayManager manager(0), std::invalid_argument);
    EXPECT_THROW(ArrayManager manager(Constants::MAX_ARRAY_SIZE + 1), std::invalid_argument);
}

TEST_F(ArrayManagerTest, GetSetElement) {
    ArrayManager manager(5);
    
    // Test valid indices
    EXPECT_NO_THROW(manager.setElement(0, 10));
    EXPECT_EQ(manager.getElement(0), 10);
    
    EXPECT_NO_THROW(manager.setElement(4, 20));
    EXPECT_EQ(manager.getElement(4), 20);
}

TEST_F(ArrayManagerTest, GetSetElementInvalidIndex) {
    ArrayManager manager(5);
    
    EXPECT_THROW(manager.getElement(-1), std::out_of_range);
    EXPECT_THROW(manager.getElement(5), std::out_of_range);
    EXPECT_THROW(manager.setElement(-1, 10), std::out_of_range);
    EXPECT_THROW(manager.setElement(5, 10), std::out_of_range);
}

TEST_F(ArrayManagerTest, InitializeWithZeros) {
    ArrayManager manager(5);
    
    // Set some values
    manager.setElement(0, 1);
    manager.setElement(2, 3);
    manager.setElement(4, 5);
    
    // Reset to zeros
    manager.initializeWithZeros();
    
    // Verify all elements are zero
    for (int i = 0; i < manager.getSize(); ++i) {
        EXPECT_EQ(manager.getElement(i), 0);
    }
}

TEST_F(ArrayManagerTest, PrintArrayNoCrash) {
    ArrayManager manager(3);
    
    // Should not crash
    EXPECT_NO_THROW(manager.printArray());
}