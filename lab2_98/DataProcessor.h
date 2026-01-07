#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "DataManager.h"
#include <unistd.h>

class DataProcessor {
private:
    static const int kMinMaxSleepMs;
    static const int kAverageSleepMs;

public:
    static void findMinMax(DataManager& dataManager);
    static void calculateAverage(DataManager& dataManager);
    static void replaceElements(DataManager& dataManager);
};

#endif