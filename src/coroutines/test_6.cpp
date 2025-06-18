#include <experimental/coroutine>

#include "core/macros.h"

struct Promise;

struct Coroutine : std::experimental::coroutine_handle<Promise> {
    using promise_type = ::Promise;
};

#define __TCPP_USE_LAZY

struct SuspendNever {
    bool await_ready() const noexcept {
        TCPP_PRINT_HERE();
        return true;
    }
    void await_suspend(std::experimental::coroutine_handle<>) const noexcept {
        TCPP_PRINT_HERE();
    }
    void await_resume() const noexcept { TCPP_PRINT_HERE(); }
};

struct SuspendAlways {
    bool await_ready() const noexcept {
        TCPP_PRINT_HERE();
        return false;
    }
    void await_suspend(std::experimental::coroutine_handle<>) const noexcept {
        TCPP_PRINT_HERE();
    }
    void await_resume() const noexcept { TCPP_PRINT_HERE(); }
};

struct Promise {
    Coroutine get_return_object() {
        TCPP_PRINT_HERE();
        return {Coroutine::from_promise(*this)};
    }
    // std::experimental::suspend_always
    SuspendAlways initial_suspend() noexcept {
        TCPP_PRINT_HERE();
        return {};
    }

    // std::experimental::suspend_always
    SuspendAlways final_suspend() noexcept {
        TCPP_PRINT_HERE();
        return {};
    }
#if 0
    void return_void() {
        TCPP_PRINT_HERE();
    }
#endif
    void return_value(std::string&& str) {
        TCPP_PRINT_HERE();
        str_ = std::move(str);
    }
    void unhandled_exception() { TCPP_PRINT_HERE(); }

    struct SuspendNeverWithValue {
        int value_;

        SuspendNeverWithValue(int value) : value_{value} {}

        bool await_ready() const noexcept {
            TCPP_PRINT_HERE();
            return true;
        }
        void await_suspend(
            std::experimental::coroutine_handle<> h) const noexcept {
            TCPP_PRINT_HERE();
        }
        int await_resume() const noexcept {
            TCPP_PRINT_HERE();
            return value_;
        }
    };
    SuspendNeverWithValue await_transform(int value) { return {value}; }

    std::string str_;
};

Coroutine Test0Coro2() {
    TCPP_PRINT_HERE();
    auto const value = co_await 100;
    ::fmt::print("value = {}\n", value);
    TCPP_PRINT_HERE();
    co_return std::string{"something else"};
}

Coroutine Test0Coro1() {
    TCPP_PRINT_HERE();
    auto h = Test0Coro2();
    TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_HERE();
    // h.resume();
    TCPP_PRINT_HERE();
    h.destroy();
    co_return std::string{"some string"};
}

void Test0() {
    TCPP_PRINT_HERE();
    Coroutine h = Test0Coro1();
    TCPP_PRINT_HERE();
    // h.resume();
    // TCPP_PRINT_HERE();
    h.resume();
    TCPP_PRINT_EXPR_RESULT(h.promise().str_);
    h.destroy();
    TCPP_PRINT_HERE();
}

#if 0
Coroutine Test1Coro() {
    TCPP_PRINT_HERE();
    co_await std::experimental::suspend_always{};
    TCPP_PRINT_HERE();
    co_await std::experimental::suspend_always{};
    TCPP_PRINT_HERE();
    co_return ;
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
    co_return ;
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
#endif

int main() {
    Test0();
#if 0
    ::fmt::print("---\n");
    Test1();
    ::fmt::print("---\n");
    Test2();
#endif

    return 0;
}
