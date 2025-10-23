#include "kolok1.h"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

TEST(FactorialTest, NormalCase) {
    std::vector<uint64_t> res = FactorialCalculator::calc(5);
    ASSERT_EQ(res.size(), 5);
    EXPECT_EQ(res[0], 1);   
    EXPECT_EQ(res[1], 2);   
    EXPECT_EQ(res[2], 6);   
    EXPECT_EQ(res[3], 24);  
    EXPECT_EQ(res[4], 120); 
}

TEST(FactorialTest, ZeroCase) {
    std::vector<uint64_t> res = FactorialCalculator::calc(0);
    EXPECT_TRUE(res.empty());
}

TEST(FactorialTest, OneCase) {
    std::vector<uint64_t> res = FactorialCalculator::calc(1);
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], 1);
}

TEST(FactorialTest, MaxCase) {
    std::vector<uint64_t> res = FactorialCalculator::calc(FactorialCalculator::MAX_N);
    ASSERT_EQ(res.size(), FactorialCalculator::MAX_N);
    EXPECT_GT(res.back(), 0); 
}

TEST(FactorialTest, NegativeCase) {
    EXPECT_THROW(FactorialCalculator::calc(-1), std::invalid_argument);
    EXPECT_THROW(FactorialCalculator::calc(-5), std::invalid_argument);
}

TEST(FactorialTest, OverflowCase) {
    EXPECT_THROW(FactorialCalculator::calc(FactorialCalculator::MAX_N + 1), 
                 std::overflow_error);
    EXPECT_THROW(FactorialCalculator::calc(100), std::overflow_error);
}

TEST(FactorialTest, CanCalcValid) {
    EXPECT_TRUE(FactorialCalculator::canCalc(0));
    EXPECT_TRUE(FactorialCalculator::canCalc(1));
    EXPECT_TRUE(FactorialCalculator::canCalc(5));
    EXPECT_TRUE(FactorialCalculator::canCalc(FactorialCalculator::MAX_N));
}

TEST(FactorialTest, CanCalcInvalid) {
    EXPECT_FALSE(FactorialCalculator::canCalc(-1));
    EXPECT_FALSE(FactorialCalculator::canCalc(-5));
    EXPECT_FALSE(FactorialCalculator::canCalc(FactorialCalculator::MAX_N + 1));
    EXPECT_FALSE(FactorialCalculator::canCalc(25));
}

TEST(OptimizedFactorialTest, Init) {
    OptimizedFactorialCalculator calc;
    EXPECT_EQ(calc.getSize(), 1); 
}

TEST(OptimizedFactorialTest, FirstCall) {
    OptimizedFactorialCalculator calc;
    std::vector<uint64_t> res = calc.calc(5);
    
    ASSERT_EQ(res.size(), 6); 
    EXPECT_EQ(res[0], 1);  
    EXPECT_EQ(res[1], 1);  
    EXPECT_EQ(res[2], 2); 
    EXPECT_EQ(res[3], 6);  
    EXPECT_EQ(res[4], 24); 
    EXPECT_EQ(res[5], 120); 
    
    EXPECT_EQ(calc.getSize(), 6);
}

TEST(OptimizedFactorialTest, UseCache) {
    OptimizedFactorialCalculator calc;
    
    std::vector<uint64_t> res1 = calc.calc(5);
    size_t firstSize = calc.getSize();
    
    std::vector<uint64_t> res2 = calc.calc(3);
    EXPECT_EQ(calc.getSize(), firstSize); 
    
    ASSERT_EQ(res2.size(), 4);
    EXPECT_EQ(res2[0], 1);
    EXPECT_EQ(res2[1], 1);
    EXPECT_EQ(res2[2], 2);
    EXPECT_EQ(res2[3], 6);
}

TEST(OptimizedFactorialTest, ExtendCache) {
    OptimizedFactorialCalculator calc;
    
    calc.calc(5);
    size_t firstSize = calc.getSize();
    
    std::vector<uint64_t> res = calc.calc(10);
    EXPECT_GT(calc.getSize(), firstSize);
    EXPECT_EQ(calc.getSize(), 11); 
    
    ASSERT_EQ(res.size(), 11);
    EXPECT_EQ(res[10], 3628800); 
}

TEST(OptimizedFactorialTest, ClearCache) {
    OptimizedFactorialCalculator calc;
    
    calc.calc(10);
    EXPECT_GT(calc.getSize(), 1);
    
    calc.clear();
    EXPECT_EQ(calc.getSize(), 1);
    
    std::vector<uint64_t> res = calc.calc(3);
    ASSERT_EQ(res.size(), 4);
    EXPECT_EQ(res[3], 6);
}

TEST(OptimizedFactorialTest, Errors) {
    OptimizedFactorialCalculator calc;
    
    EXPECT_THROW(calc.calc(-1), std::invalid_argument);
    EXPECT_THROW(calc.calc(FactorialCalculator::MAX_N + 1), 
                 std::overflow_error);
}

TEST(FactorialTest, Boundaries) {
    std::vector<uint64_t> res0 = FactorialCalculator::calc(0);
    EXPECT_TRUE(res0.empty());
    
    std::vector<uint64_t> res1 = FactorialCalculator::calc(1);
    ASSERT_EQ(res1.size(), 1);
    EXPECT_EQ(res1[0], 1);
    
    EXPECT_NO_THROW(FactorialCalculator::calc(FactorialCalculator::MAX_N));
    
    EXPECT_THROW(FactorialCalculator::calc(FactorialCalculator::MAX_N + 1), 
                 std::overflow_error);
}

TEST(FactorialTest, CorrectValues) {
    std::vector<uint64_t> expected = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800};
    
    std::vector<uint64_t> res1 = FactorialCalculator::calc(10);
    ASSERT_EQ(res1.size(), 10);
    
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(res1[i], expected[i + 1]) << "Mismatch at position " << i;
    }
    
    OptimizedFactorialCalculator calc;
    std::vector<uint64_t> res2 = calc.calc(10);
    ASSERT_EQ(res2.size(), 11);
    
    for (int i = 0; i < 11; ++i) {
        EXPECT_EQ(res2[i], expected[i]) << "Mismatch at position " << i;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}