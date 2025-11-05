#include "ArrayManager.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include "Constants.h"

ArrayManager::ArrayManager(int size) : m_array(NULL), m_size(0) {
    if (size < Constants::MIN_ARRAY_SIZE || size > Constants::MAX_ARRAY_SIZE) {
        throw std::invalid_argument("Invalid array size");
    }
    
    m_array = new int[size];
    if (m_array == NULL) {
        throw std::runtime_error("Memory allocation failed");
    }
    
    m_size = size;
    initializeWithZeros();
}

ArrayManager::~ArrayManager() {
    delete[] m_array;
}

int ArrayManager::getSize() const {
    return m_size;
}

int ArrayManager::getElement(int index) const {
    if (index < 0 || index >= m_size) {
        throw std::out_of_range("Array index out of bounds");
    }
    return m_array[index];
}

void ArrayManager::setElement(int index, int value) {
    if (index < 0 || index >= m_size) {
        throw std::out_of_range("Array index out of bounds");
    }
    m_array[index] = value;
}

void ArrayManager::initializeWithZeros() {
    std::memset(m_array, 0, m_size * sizeof(int));
}

void ArrayManager::printArray() const {
    for (int i = 0; i < m_size; ++i) {
        std::cout << m_array[i] << " ";
    }
    std::cout << std::endl;
}