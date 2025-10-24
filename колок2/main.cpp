#include "kolok2.h"
#include <iostream>

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