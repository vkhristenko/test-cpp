#include <experimental/coroutine>

#include "core/macros.h"

struct Promise;

struct Coroutine : std::experimental::coroutine_handle<Promise> {
    using promise_type = ::Promise;
};

struct Promise {
    Coroutine get_return_object() { return {Coroutine::from_promise(*this)}; }
    std::experimental::suspend_always initial_suspend() noexcept { return {}; }
    std::experimental::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
};

Coroutine Test0Coro() {
    TCPP_PRINT_HERE();
    co_return;
}

void Test0() {
    TCPP_PRINT_HERE();
    Coroutine h = Test0Coro();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    h.destroy();
    TCPP_PRINT_HERE();
}

Coroutine Test1Coro() {
    TCPP_PRINT_HERE();
    co_await std::experimental::suspend_always{};
    TCPP_PRINT_HERE();
    co_await std::experimental::suspend_always{};
    TCPP_PRINT_HERE();
    co_return;
}

void Test1() {
    TCPP_PRINT_HERE();
    Coroutine h = Test1Coro();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    h.destroy();
}

struct Promise2;

struct Coroutine2 : std::experimental::coroutine_handle<Promise2> {
    using promise_type = ::Promise2;
};

struct Promise2 {
    Coroutine2 get_return_object() { return {Coroutine2::from_promise(*this)}; }
    std::experimental::suspend_always initial_suspend() noexcept { return {}; }
    std::experimental::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
    std::experimental::suspend_always await_transform(int value) {
        TCPP_PRINT_HERE();
        return {};
    }
};

Coroutine2 Test2Coro() {
    TCPP_PRINT_HERE();
    co_await 10;
    TCPP_PRINT_HERE();
    co_await 20;
    TCPP_PRINT_HERE();
    co_return;
}

void Test2() {
    TCPP_PRINT_HERE();
    Coroutine2 h = Test2Coro();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    h.destroy();
}

int main() {
    Test0();
    ::fmt::print("---\n");
    Test1();
    ::fmt::print("---\n");
    Test2();

    return 0;
}
