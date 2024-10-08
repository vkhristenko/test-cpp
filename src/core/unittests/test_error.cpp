#include "core/error.h"

#include "gtest/gtest.h"

using namespace core;

TEST(ErrorTest, StringErrorDetailsTest) {
    auto e = TCPP_MAKE_ERROR(StringErrorDetails{"some error"});
    ASSERT_EQ( e.IsA<StringErrorDetails>(), true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
