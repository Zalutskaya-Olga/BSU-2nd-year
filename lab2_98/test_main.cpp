#include <gtest/gtest.h>
#include "ThreadManager.h"
#include <vector>
#include <cmath>
#include <iostream>

class ThreadManagerTest : public ::testing::Test {
protected:
    void SetUp() {
        int test_array[] = {5, 2, 8, 1, 9, 3};
        test_data_ = std::vector<int>(test_array, test_array + 6);
    }

    void TearDown() {
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
    
    double expected_avg = (5.0 + 2.0 + 8.0 + 1.0 + 9.0 + 3.0) / 6.0;
    EXPECT_DOUBLE_EQ(manager.getAverageValue(), expected_avg);
}

TEST_F(ThreadManagerTest, ReplaceElements) {
    ThreadManager manager;
    manager.setData(test_data_);
    
    manager.findMinMax();
    manager.calculateAverage();
    
    manager.replaceElements();
    
    const std::vector<int>& modified_data = manager.getData();
    double avg = manager.getAverageValue();
    int min_val = manager.getMinValue();
    int max_val = manager.getMaxValue();
    
    for (size_t i = 0; i < modified_data.size(); ++i) {
        int value = modified_data[i];
        if (value == min_val || value == max_val) {
            EXPECT_EQ(value, static_cast<int>(avg));
        }
    }
}

TEST_F(ThreadManagerTest, EmptyArray) {
    ThreadManager manager;
    manager.setData(std::vector<int>());
    
    manager.findMinMax();
    manager.calculateAverage();
    manager.replaceElements();
}

TEST_F(ThreadManagerTest, SingleElement) {
    ThreadManager manager;
    int single_array[] = {42};
    manager.setData(std::vector<int>(single_array, single_array + 1));
    
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
    int negative_array[] = {-5, -2, -8, -1, -9};
    std::vector<int> negative_data(negative_array, negative_array + 5);
    manager.setData(negative_data);
    
    manager.findMinMax();
    manager.calculateAverage();
    
    EXPECT_EQ(manager.getMinValue(), -9);
    EXPECT_EQ(manager.getMaxValue(), -1);
    
    double expected_avg = (-5.0 - 2.0 - 8.0 - 1.0 - 9.0) / 5.0;
    EXPECT_DOUBLE_EQ(manager.getAverageValue(), expected_avg);
}

TEST_F(ThreadManagerTest, AllSameElements) {
    ThreadManager manager;
    int same_array[] = {5, 5, 5, 5, 5};
    std::vector<int> same_data(same_array, same_array + 5);
    manager.setData(same_data);
    
    manager.findMinMax();
    manager.calculateAverage();
    manager.replaceElements();
    
    EXPECT_EQ(manager.getMinValue(), 5);
    EXPECT_EQ(manager.getMaxValue(), 5);
    EXPECT_DOUBLE_EQ(manager.getAverageValue(), 5.0);
    
    const std::vector<int>& data = manager.getData();
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(data[i], 5);
    }
}