#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include "DataManager.h"
#include "DataProcessor.h"
#include "ThreadController.h"

class ThreadManager {
private:
    DataManager data_manager_;
    ThreadController thread_controller_;

    ThreadManager(const ThreadManager&);
    ThreadManager& operator=(const ThreadManager&);

public:
    ThreadManager() : thread_controller_(data_manager_) {}

    void execute();

   
    void findMinMax() { 
        DataProcessor::findMinMax(data_manager_); 
    }
    
    void calculateAverage() { 
        DataProcessor::calculateAverage(data_manager_); 
    }
    
    void replaceElements() { 
        DataProcessor::replaceElements(data_manager_); 
    }

    
    const std::vector<int>& getData() const { return data_manager_.getData(); }
    int getMinValue() const { return data_manager_.getMinValue(); }
    int getMaxValue() const { return data_manager_.getMaxValue(); }
    double getAverageValue() const { return data_manager_.getAverageValue(); }
    
    void setData(const std::vector<int>& data) { data_manager_.setData(data); }
};

#endif
