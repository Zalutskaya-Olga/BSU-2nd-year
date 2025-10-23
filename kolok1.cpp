#include "kolok1.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>

using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::invalid_argument;
using std::overflow_error;

void checkInput(int n) {
    if (n < 0) {
        throw invalid_argument("n must be non-negative");
    }
    if (n > FactorialCalculator::MAX_N) {
        throw overflow_error("Factorial would cause overflow");
    }
}

vector<uint64_t> FactorialCalculator::calc(int n) {
    checkInput(n);
    
    vector<uint64_t> res;
    uint64_t fact = 1;
    
    res.reserve(static_cast<size_t>(n));
    
    for (int i = 1; i <= n; ++i) {
        fact *= static_cast<uint64_t>(i);
        res.push_back(fact);
    }
    
    return res;
}

bool FactorialCalculator::canCalc(int n) {
    return (n >= 0 && n <= MAX_N);
}

OptimizedFactorialCalculator::OptimizedFactorialCalculator() 
    : cache(1, 1), maxN(0) {
}

vector<uint64_t> OptimizedFactorialCalculator::calc(int n) {
    checkInput(n);
    
    if (n > maxN) {
        extend(n);
    }
    
    return vector<uint64_t>(cache.begin(), cache.begin() + n + 1);
}

void OptimizedFactorialCalculator::clear() {
    vector<uint64_t> newCache(1, 1);
    cache.swap(newCache);
    maxN = 0;
}

size_t OptimizedFactorialCalculator::getSize() const {
    return cache.size();
}

void OptimizedFactorialCalculator::extend(int n) {
    if (static_cast<size_t>(n) >= cache.capacity()) {
        cache.reserve(static_cast<size_t>(n + 1));
    }
    
    for (int i = maxN + 1; i <= n; ++i) {
        uint64_t next = cache[i-1] * static_cast<uint64_t>(i);
        cache.push_back(next);
    }
    maxN = n;
}

void print(const vector<uint64_t>& facts) {
    for (size_t i = 0; i < facts.size(); ++i) {
        cout << i << "! = " << facts[i] << endl;
    }
}

int main() {
    cout << "FactorialCalculator demo:" << endl;
    try {
        vector<uint64_t> res1 = FactorialCalculator::calc(5);
        print(res1);
    } catch (const std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    cout << "\nOptimizedFactorialCalculator demo:" << endl;
    OptimizedFactorialCalculator optCalc;
    
    try {
        vector<uint64_t> res2 = optCalc.calc(7);
        print(res2);
        cout << "Cache size: " << optCalc.getSize() << endl;
        
        vector<uint64_t> res3 = optCalc.calc(10);
        cout << "After extending to 10, cache size: " << optCalc.getSize() << endl;
        
    } catch (const std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    cout << "\nError handling demo:" << endl;
    try {
        FactorialCalculator::calc(-1);
    } catch (const std::exception& e) {
        cout << "Expected error for negative input: " << e.what() << endl;
    }
    
    try {
        FactorialCalculator::calc(25);
    } catch (const std::exception& e) {
        cout << "Expected error for overflow: " << e.what() << endl;
    }
    
    return 0;
}