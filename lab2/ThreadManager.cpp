#include "ThreadManager.h"

void ThreadManager::execute() {
    data_manager_.readInputData();
    data_manager_.printArray("Original array");

    thread_controller_.executeParallelComputations();

    DataProcessor::replaceElements(data_manager_);
    data_manager_.printArray("Modified array");
}
