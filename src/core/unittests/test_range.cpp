#include <gtest/gtest.h>

#include <iostream>

#include "core/range.h"

using namespace core;

TEST(RangeTest, BasicIteration) {
    Range<int> r = Range(1, 5);
    int expected[] = {1, 2, 3, 4};
    int i = 0;
    for (int x : r) {
        EXPECT_EQ(x, expected[i++]);
    }
}

TEST(RangeTest, EmptyRange) {
    Range<int> r = Range(5, 5);
    int i = 0;
    for (int x : r) {
        FAIL() << "Empty Range should not iterate";
    }
}

TEST(RangeTest, ReverseIteration) {
    Range<int> r = Range(5, 1);
    int expected[] = {5, 4, 3, 2};
    int i = 0;
    for (int x : r) {
        EXPECT_EQ(x, expected[i++]);
    }
}

TEST(RangeTest, UnsignedIteration) {
    Range<unsigned int> r = Range(0u, 5u);
    unsigned int expected[] = {0u, 1u, 2u, 3u, 4u};
    unsigned int i = 0u;
    for (unsigned int x : r) {
        EXPECT_EQ(x, expected[i++]);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
