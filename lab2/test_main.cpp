#include <gtest/gtest.h>
#include "ThreadManager.h"
#include <vector>
#include <cmath>

class ThreadManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_data_ = {5, 2, 8, 1, 9, 3};
    }

    void TearDown() override {
    }

    std::vector<int> test_data_;
};

TEST_F(ThreadManagerTest, FindMinMax) {
    ThreadManager manager;
    manager.setData(test_data_);
    
    manager.findMinMax();
    
    EXPECT_EQ(manager.getMinValue(), 1);
    EXPECT_EQ(manager.getMaxValue(), 9);
}

TEST_F(ThreadManagerTest, CalculateAverage) {
    ThreadManager manager;
    manager.setData(test_data_);
    
    manager.calculateAverage();
    
    double expected_avg = (5 + 2 + 8 + 1 + 9 + 3) / 6.0;
    EXPECT_DOUBLE_EQ(manager.getAverageValue(), expected_avg);
}

TEST_F(ThreadManagerTest, ReplaceElements) {
    ThreadManager manager;
    manager.setData(test_data_);
    
    manager.findMinMax();
    manager.calculateAverage();
    
    manager.replaceElements();
    
    const auto& modified_data = manager.getData();
    double avg = manager.getAverageValue();
    int min_val = manager.getMinValue();
    int max_val = manager.getMaxValue();
    
    for (int value : modified_data) {
        if (value == min_val || value == max_val) {
            EXPECT_EQ(value, static_cast<int>(avg));
        }
    }
}

TEST_F(ThreadManagerTest, EmptyArray) {
    ThreadManager manager;
    manager.setData({});
    
    manager.findMinMax();
    manager.calculateAverage();
    manager.replaceElements();
}

TEST_F(ThreadManagerTest, SingleElement) {
    ThreadManager manager;
    manager.setData({42});
    
    manager.findMinMax();
    manager.calculateAverage();
    
    EXPECT_EQ(manager.getMinValue(), 42);
    EXPECT_EQ(manager.getMaxValue(), 42);
    EXPECT_DOUBLE_EQ(manager.getAverageValue(), 42.0);
    
    manager.replaceElements();
    
    EXPECT_EQ(manager.getData()[0], 42);
}

TEST_F(ThreadManagerTest, NegativeNumbers) {
    ThreadManager manager;
    std::vector<int> negative_data = {-5, -2, -8, -1, -9};
    manager.setData(negative_data);
    
    manager.findMinMax();
    manager.calculateAverage();
    
    EXPECT_EQ(manager.getMinValue(), -9);
    EXPECT_EQ(manager.getMaxValue(), -1);
    
    double expected_avg = (-5 - 2 - 8 - 1 - 9) / 5.0;
    EXPECT_DOUBLE_EQ(manager.getAverageValue(), expected_avg);
}

TEST_F(ThreadManagerTest, AllSameElements) {
    ThreadManager manager;
    std::vector<int> same_data = {5, 5, 5, 5, 5};
    manager.setData(same_data);
    
    manager.findMinMax();
    manager.calculateAverage();
    manager.replaceElements();
    
    EXPECT_EQ(manager.getMinValue(), 5);
    EXPECT_EQ(manager.getMaxValue(), 5);
    EXPECT_DOUBLE_EQ(manager.getAverageValue(), 5.0);
    
    for (int value : manager.getData()) {
        EXPECT_EQ(value, 5);
    }
}
