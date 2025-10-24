#include "kolok2.h"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

class ArrayUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        basicArray = {1, 2, 3, 2, 1, 4, 5};
        expectedBasic = {1, 2, 3, 4, 5};
        
        noDuplicates = {1, 2, 3, 4, 5};
        expectedNoDuplicates = {1, 2, 3, 4, 5};
        
        allDuplicates = {1, 1, 1, 1, 1};
        expectedAllDuplicates = {1};
        
        singleElement = {42};
        expectedSingleElement = {42};
        
        negativeNumbers = {-1, -2, -1, -3, -2};
        expectedNegativeNumbers = {-1, -2, -3};
        
        mixedNumbers = {0, -1, 1, 0, -1, 1};
        expectedMixedNumbers = {0, -1, 1};
    }

    std::vector<int> basicArray;
    std::vector<int> expectedBasic;
    std::vector<int> noDuplicates;
    std::vector<int> expectedNoDuplicates;
    std::vector<int> allDuplicates;
    std::vector<int> expectedAllDuplicates;
    std::vector<int> singleElement;
    std::vector<int> expectedSingleElement;
    std::vector<int> negativeNumbers;
    std::vector<int> expectedNegativeNumbers;
    std::vector<int> mixedNumbers;
    std::vector<int> expectedMixedNumbers;
};

TEST_F(ArrayUtilsTest, RemoveDuplicatesBasic) {
    std::vector<int> result = ArrayUtils::removeDuplicates(basicArray);
    EXPECT_EQ(result, expectedBasic);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesNoDuplicates) {
    std::vector<int> result = ArrayUtils::removeDuplicates(noDuplicates);
    EXPECT_EQ(result, expectedNoDuplicates);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesAllSame) {
    std::vector<int> result = ArrayUtils::removeDuplicates(allDuplicates);
    EXPECT_EQ(result, expectedAllDuplicates);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesSingleElement) {
    std::vector<int> result = ArrayUtils::removeDuplicates(singleElement);
    EXPECT_EQ(result, expectedSingleElement);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesNegativeNumbers) {
    std::vector<int> result = ArrayUtils::removeDuplicates(negativeNumbers);
    EXPECT_EQ(result, expectedNegativeNumbers);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesMixedNumbers) {
    std::vector<int> result = ArrayUtils::removeDuplicates(mixedNumbers);
    EXPECT_EQ(result, expectedMixedNumbers);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesEmptyArray) {
    std::vector<int> emptyArray;
    EXPECT_THROW(ArrayUtils::removeDuplicates(emptyArray), std::invalid_argument);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesPreservesOrder) {
    std::vector<int> array = {3, 1, 2, 3, 1, 4, 2};
    std::vector<int> expected = {3, 1, 2, 4};
    std::vector<int> result = ArrayUtils::removeDuplicates(array);
    EXPECT_EQ(result, expected);
}

TEST_F(ArrayUtilsTest, RemoveDuplicatesLargeArray) {
    std::vector<int> largeArray;
    std::vector<int> expected;
    
    for (int i = 0; i < 1000; ++i) {
        largeArray.push_back(i % 100); 
        if (i < 100) {
            expected.push_back(i);
        }
    }
    
    std::vector<int> result = ArrayUtils::removeDuplicates(largeArray);
    EXPECT_EQ(result, expected);
}

TEST_F(ArrayUtilsTest, GetVersionTest) {
    std::string version = ArrayUtils::getVersion();
    EXPECT_EQ(version, "1.0.0");
    EXPECT_FALSE(version.empty());
}

TEST_F(ArrayUtilsTest, ValidateArrayTest) {
    EXPECT_TRUE(ArrayUtils::validateArray({1, 2, 3}));
    EXPECT_TRUE(ArrayUtils::validateArray({1}));
    EXPECT_FALSE(ArrayUtils::validateArray(std::vector<int>()));
}

TEST_F(ArrayUtilsTest, BoundaryValues) {
    std::vector<int> minValue = {INT_MIN, INT_MIN, 0};
    std::vector<int> expectedMin = {INT_MIN, 0};
    EXPECT_EQ(ArrayUtils::removeDuplicates(minValue), expectedMin);
    
    std::vector<int> maxValue = {INT_MAX, INT_MAX, 0};
    std::vector<int> expectedMax = {INT_MAX, 0};
    EXPECT_EQ(ArrayUtils::removeDuplicates(maxValue), expectedMax);
}

class RemoveDuplicatesParamTest : public ::testing::TestWithParam<std::tuple<std::vector<int>, std::vector<int>>> {
};

TEST_P(RemoveDuplicatesParamTest, VariousCases) {
    auto input = std::get<0>(GetParam());
    auto expected = std::get<1>(GetParam());
    auto result = ArrayUtils::removeDuplicates(input);
    EXPECT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(
    RemoveDuplicatesVariations,
    RemoveDuplicatesParamTest,
    ::testing::Values(
        std::make_tuple(std::vector<int>{1, 2, 3}, std::vector<int>{1, 2, 3}),
        std::make_tuple(std::vector<int>{1, 1, 1}, std::vector<int>{1}),
        std::make_tuple(std::vector<int>{2, 1, 2}, std::vector<int>{2, 1}),
        std::make_tuple(std::vector<int>{5, 4, 3, 2, 1}, std::vector<int>{5, 4, 3, 2, 1}),
        std::make_tuple(std::vector<int>{1, 2, 3, 2, 1}, std::vector<int>{1, 2, 3})
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}