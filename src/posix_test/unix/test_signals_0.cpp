#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

#include "fmt/core.h"

void handler(int s) noexcept {
    ::fmt::print("interrupt signal {}\n", s);
    std::exit(s);
}

void Test0() {
    signal(SIGINT, handler);

    while (1) {
        ::fmt::print("sleeping...\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    Test0();

    return 0;
}
