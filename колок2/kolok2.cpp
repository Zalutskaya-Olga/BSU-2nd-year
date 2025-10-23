#include "kolok2.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>
#include <iterator>

const char* const ArrayUtils::LIBRARY_VERSION = "1.0.0";

ArrayUtils::ArrayUtils() {
}

std::vector<int> ArrayUtils::removeDuplicates(const std::vector<int>& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input array cannot be empty");
    }
    
    if (!validateArray(input)) {
        throw std::invalid_argument("Input array contains invalid values");
    }
    
    std::vector<int> result;
    std::set<int> seenElements;
    
    try {
        result.reserve(input.size());
        
        for (std::vector<int>::const_iterator it = input.begin(); 
             it != input.end(); ++it) {
            if (seenElements.find(*it) == seenElements.end()) {
                result.push_back(*it);
                seenElements.insert(*it);
            }
        }
        
        std::vector<int>(result).swap(result);
        
    } catch (const std::bad_alloc& e) {
        std::vector<int> empty;
        empty.swap(result);
        throw std::bad_alloc();
    } catch (const std::exception& e) {
        std::vector<int> empty;
        empty.swap(result);
        throw;
    }
    
    return result;
}

bool ArrayUtils::validateArray(const std::vector<int>& input) {
    return !input.empty();
}

std::string ArrayUtils::getVersion() {
    return std::string(LIBRARY_VERSION);
}

int main() {
    try {
        std::vector<int> inputArray;
        int n, value;
        
        std::cout << "Enter the number of elements: ";
        std::cin >> n;
        
        if (n <= 0) {
            std::cout << "Number of elements must be positive!" << std::endl;
            return 1;
        }
        
        std::cout << "Enter " << n << " integers:" << std::endl;
        for (int i = 0; i < n; ++i) {
            std::cout << "Element " << (i + 1) << ": ";
            std::cin >> value;
            inputArray.push_back(value);
        }
        
        std::vector<int> result = ArrayUtils::removeDuplicates(inputArray);
        
        std::cout << "Original array: ";
        for (size_t i = 0; i < inputArray.size(); ++i) {
            std::cout << inputArray[i] << " ";
        }
        std::cout << std::endl;
        
        std::cout << "Array after removing duplicates: ";
        for (size_t i = 0; i < result.size(); ++i) {
            std::cout << result[i] << " ";
        }
        std::cout << std::endl;
        
        std::cout << "Library version: " << ArrayUtils::getVersion() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}