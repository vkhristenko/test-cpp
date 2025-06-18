#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

#include "core/par_ostream.h"
#include "fmt/core.h"

using core::ParOStream;

#define SIGNAL_CASE(STRSIG) \
    case STRSIG:            \
        return #STRSIG

constexpr std::string_view SignalToString(int s) {
    switch (s) {
        SIGNAL_CASE(SIGINT);
        SIGNAL_CASE(SIGABRT);
        default:
            return "UNKNOWN";
    }

    return "UNKNOWN";
}

void handler(int s) noexcept {
    ParOStream{std::cout} << "interrupt signal " << s
                          << " caught from thread id "
                          << std::this_thread::get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(5));
    ParOStream{std::cout} << "exiting via thread id "
                          << std::this_thread::get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::exit(s);
}

void ThreadRunner() {
    while (1) {
        ParOStream{std::cout} << "sleeping from " << std::this_thread::get_id()
                              << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ThreadToAbort() {
    ParOStream{std::cout} << "sleeping from " << std::this_thread::get_id()
                          << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::abort();
}

void Test0() {
    signal(SIGINT, handler);

    std::vector<std::thread> ts;
    for (int i = 0; i < 4; i++) {
        ts.emplace_back([] { ThreadRunner(); });
    }

    std::thread __ta{[] { ThreadToAbort(); }};

    for (auto& t : ts) {
        t.join();
    }
    __ta.join();
}

int main() {
    Test0();

    return 0;
}
