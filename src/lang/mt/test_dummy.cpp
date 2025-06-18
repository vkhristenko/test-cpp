#include <iostream>
#include <sstream>
#include <thread>

#include "core/macros.h"
#include "fmt/core.h"

namespace fmt {

template <>
struct formatter<std::thread::id> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(std::thread::id const& id, FormatContext& ctx) {
        std::stringstream ss;
        ss << id;
        return format_to(ctx.out(), "{}", ss.str());
    }
};

}  // namespace fmt

void Test0() {
    std::thread t{[] {
        ::fmt::print("hello from thread {}\n", std::this_thread::get_id());
    }};
    t.join();
}

void Test1() {
    std::thread t{[] {
        ::fmt::print("hello from thread {}\n", std::this_thread::get_id());
    }};
    auto a = std::move(t);
    a.join();
}

int main() {
    Test0();
    Test1();

    return 0;
}
