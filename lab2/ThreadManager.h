#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <limits>
#include <stdexcept>

class ThreadManager {
private:
    static const int kMinMaxSleepMs = 7;
    static const int kAverageSleepMs = 12;
    
    std::vector<int> data_;
    int min_value_;
    int max_value_;
    double average_value_;

    ThreadManager(const ThreadManager&);
    ThreadManager& operator=(const ThreadManager&);

public:
    ThreadManager() : min_value_(0), max_value_(0), average_value_(0.0) {}

    void readInputData();
    static void* minMaxThread(void* arg);
    static void* averageThread(void* arg);
    void findMinMax();
    void calculateAverage();
    void replaceElements();
    void printArray(const std::string& title) const;
    void execute();

    const std::vector<int>& getData() const { return data_; }
    int getMinValue() const { return min_value_; }
    int getMaxValue() const { return max_value_; }
    double getAverageValue() const { return average_value_; }
    
    void setData(const std::vector<int>& data) { data_ = data; }
};

#endif