#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include <pthread.h>
#include <stdexcept>
#include "DataManager.h"
#include "DataProcessor.h"

class ThreadController {
private:
    DataManager& data_manager_;

    static void* minMaxThread(void* arg);
    static void* averageThread(void* arg);

public:
    explicit ThreadController(DataManager& dataManager) : data_manager_(dataManager) {}

    void executeParallelComputations();
};

#endif