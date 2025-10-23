#ifndef KOLOK1_H
#define KOLOK1_H

#include <vector>
#include <stdexcept>

typedef unsigned long long uint64_t;

class FactorialCalculator {
public:
    static std::vector<uint64_t> calc(int n);
    static bool canCalc(int n);
    
    static const int MAX_N = 20;
};

class OptimizedFactorialCalculator {
public:
    OptimizedFactorialCalculator();
    std::vector<uint64_t> calc(int n);
    void clear();
    size_t getSize() const;

private:
    std::vector<uint64_t> cache;
    int maxN;
    
    void extend(int n);
};

void print(const std::vector<uint64_t>& facts);

#endif