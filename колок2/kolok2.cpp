#include "kolok2.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>
#include <iterator>
#include <climits>  

const char* const ArrayUtils::LIBRARY_VERSION = "1.0.0";

ArrayUtils::ArrayUtils() {
}

std::vector<int> ArrayUtils::removeDuplicates(const std::vector<int>& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input array cannot be empty");
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

        result.shrink_to_fit();
        
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
