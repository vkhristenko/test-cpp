#include <gtest/gtest.h>

#include <iostream>

#include "core/small_type_erased_base.h"

using namespace core;

TEST(SmallTypeErasedTest, First) {
    SmallTypeErasedBase<64> base1;
    EXPECT_FALSE(base1.has_value()) << "no value";

    SmallTypeErasedBase<64> base2 = []() {};
    EXPECT_TRUE(base2.has_value()) << "has value";

    base1 = []() {};
    EXPECT_TRUE(base1.has_value()) << "has value";

    // TODO understand if base should destruct right away or wait for raii
    base2 = std::move(base1);
    EXPECT_TRUE(base1.has_value()) << "has value";
    EXPECT_TRUE(base2.has_value()) << "has value";
}
