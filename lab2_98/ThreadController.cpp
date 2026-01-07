#include "ThreadController.h"

void* ThreadController::minMaxThread(void* arg) {
    ThreadController* controller = static_cast<ThreadController*>(arg);
    DataProcessor::findMinMax(controller->data_manager_);
    return NULL;
}

void* ThreadController::averageThread(void* arg) {
    ThreadController* controller = static_cast<ThreadController*>(arg);
    DataProcessor::calculateAverage(controller->data_manager_);
    return NULL;
}

void ThreadController::executeParallelComputations() {
    pthread_t min_max_thread, average_thread;
    
    if (pthread_create(&min_max_thread, NULL, minMaxThread, this) != 0) {
        throw std::runtime_error("Cannot create min_max thread");
    }
    
    if (pthread_create(&average_thread, NULL, averageThread, this) != 0) {
        pthread_join(min_max_thread, NULL);
        throw std::runtime_error("Cannot create average thread");
    }

    pthread_join(min_max_thread, NULL);
    pthread_join(average_thread, NULL);
}