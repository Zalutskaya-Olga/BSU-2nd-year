#include "DataProcessor.h"

void DataProcessor::findMinMax(DataManager& dataManager) {
    const auto& data = dataManager.getData();
    if (data.empty()) return;
    
    int min_value = data[0];
    int max_value = data[0];

    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] < min_value) min_value = data[i];
        if (data[i] > max_value) max_value = data[i];
        usleep(kMinMaxSleepMs * 1000);
    }
    
    dataManager.setMinValue(min_value);
    dataManager.setMaxValue(max_value);
    std::cout << "Min: " << min_value << "\nMax: " << max_value << std::endl;
}

void DataProcessor::calculateAverage(DataManager& dataManager) {
    const auto& data = dataManager.getData();
    if (data.empty()) return;
    
    long long sum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i];
        usleep(kAverageSleepMs * 1000);
    }
    
    double average_value = static_cast<double>(sum) / data.size();
    dataManager.setAverageValue(average_value);
    std::cout << "Average: " << average_value << std::endl;
}

void DataProcessor::replaceElements(DataManager& dataManager) {
    auto& data = const_cast<std::vector<int>&>(dataManager.getData());
    int min_value = dataManager.getMinValue();
    int max_value = dataManager.getMaxValue();
    double average_value = dataManager.getAverageValue();
    
    int replace_count = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == min_value || data[i] == max_value) {
            data[i] = static_cast<int>(average_value);
            ++replace_count;
        }
    }
    std::cout << "Replaced " << replace_count << " elements" << std::endl;
}