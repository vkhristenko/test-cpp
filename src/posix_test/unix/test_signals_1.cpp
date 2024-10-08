#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <vector>

#include "fmt/core.h"

#include "core/par_ostream.h"

using core::ParOStream;

void handler(int s) noexcept {
    ParOStream{std::cout} << "interrupt signal " << s << " caught from thread id "
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

void Test0() {
    signal(SIGINT, handler);

    std::vector<std::thread> ts;
    for (int i=0; i<4; i++) {
        ts.emplace_back([]{ThreadRunner();});
    }

    for (auto& t : ts) {
        t.join();
    }
}

int main() {
    Test0();

    return 0;
}
