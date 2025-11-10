#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <vector>
#include <iostream>

class DataManager {
private:
    std::vector<int> data_;
    int min_value_;
    int max_value_;
    double average_value_;

public:
    DataManager() : min_value_(0), max_value_(0), average_value_(0.0) {}

    void readInputData();
    void setData(const std::vector<int>& data) { data_ = data; }
    const std::vector<int>& getData() const { return data_; }
    
    int getMinValue() const { return min_value_; }
    int getMaxValue() const { return max_value_; }
    double getAverageValue() const { return average_value_; }
    
    void setMinValue(int min) { min_value_ = min; }
    void setMaxValue(int max) { max_value_ = max; }
    void setAverageValue(double avg) { average_value_ = avg; }
 
    void printArray(const std::string& title) const;
};

#endif