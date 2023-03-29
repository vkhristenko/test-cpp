#include <iostream>

#include "fmt/core.h"

// #include "boost/date_time/gregorian/gregorian.hpp"

void TestFmt() noexcept { ::fmt::print("hello world!\n"); }

void TestBoost() noexcept {
    //::boost::date_time
}

int main() {
    TestBoost();
    TestFmt();
    return 0;
}
