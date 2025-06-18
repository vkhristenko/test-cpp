#include <gtest/gtest.h>

#include <iostream>

#include "core/result.h"

using namespace core;

#if 0
TEST(ResultTest, Unwrap) {
    Result<int, std::string> ok = 42;
    Result<int, std::string> err = "error message";

    ASSERT_EQ(ok.unwrap(), 42);

    ASSERT_THROW(err.unwrap(), std::string);
}

TEST(ResultTest, UnwrapOr) {
    Result<int, std::string> ok = 42;
    Result<int, std::string> err = "error message";

    ASSERT_EQ(ok.unwrap_or(0), 42);
    ASSERT_EQ(err.unwrap_or(0), 0);
}

TEST(ResultTest, IsOk) {
    Result<int, std::string> ok = 42;
    Result<int, std::string> err = "error message";

    ASSERT_TRUE(ok.is_ok());
    ASSERT_FALSE(err.is_ok());
}

TEST(ResultTest, IsErr) {
    Result<int, std::string> ok = 42;
    Result<int, std::string> err = "error message";

    ASSERT_FALSE(ok.is_err());
    ASSERT_TRUE(err.is_err());
}

TEST(ResultTest, UnwrapUnsafe) {
    Result<int, std::string> ok = 42;
    Result<int, std::string> err = "error message";

    ASSERT_EQ(ok.unwrap_unsafe(), 42);

    // Segfaults, but doesn't crash the program
    ASSERT_DEATH(err.unwrap_unsafe(), "");
}

TEST(ResultTest, UnwrapErrUnsafe) {
    Result<int, std::string> ok = 42;
    Result<int, std::string> err = "error message";

        // Segfaults, but doesn't crash the program
        ASSERT_DEATH(ok.unwrap_err_unsafe(), "");

    ASSERT_EQ(err.unwrap_err_unsafe(), "error message");
}
#endif
