#include "DataManager.h"

void DataManager::readInputData() {
    int size = 0;
    std::cout << "Enter array size: ";
    std::cin >> size;
    
    if (size <= 0) {
        throw std::invalid_argument("Array size must be positive");
    }
    
    data_.resize(size);
    std::cout << "Enter " << size << " elements:" << std::endl;
    for (int i = 0; i < size; ++i) {
        if (!(std::cin >> data_[i])) {
            throw std::invalid_argument("Invalid input");
        }
    }
}

void DataManager::printArray(const std::string& title) const {
    std::cout << title << ": ";
    for (size_t i = 0; i < data_.size(); ++i) {
        std::cout << data_[i] << " ";
    }
    std::cout << std::endl;
}