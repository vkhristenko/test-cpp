#include <experimental/coroutine>

#include "core/macros.h"

// #define TCPP_PRINT_HERE()

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

template <typename T>
struct AwaitablePromise;

template <typename T>
struct Awaitable {
    using promise_type = AwaitablePromise<T>;
    Awaitable(Awaitable const&) = delete;
    Awaitable& operator=(Awaitable const&) = delete;
    Awaitable(Awaitable&&) = default;
    Awaitable& operator=(Awaitable&&) = default;
    Awaitable() { this_coro_.destroy(); }

    Awaitable(std::experimental::coroutine_handle<AwaitablePromise<T>> h)
        : this_coro_{std::move(h)} {
        TCPP_PRINT_HERE();
    }

    bool await_ready() const noexcept {
        TCPP_PRINT_HERE();
        return false;
    }
    auto await_suspend(std::experimental::coroutine_handle<> h) const noexcept {
        TCPP_PRINT_HERE();
        return h;
    }
    T await_resume() const noexcept {
        TCPP_PRINT_HERE();
        return std::move(this_coro_.promise().value_);
    }

    std::experimental::coroutine_handle<AwaitablePromise<T>> this_coro_;
};

template <typename T>
struct AwaitablePromise {
    Awaitable<T> get_return_object() {
        TCPP_PRINT_HERE();
        return Awaitable<T>{std::experimental::coroutine_handle<
            AwaitablePromise<T>>::from_promise(*this)};
    }

    SuspendNever initial_suspend() noexcept {
        TCPP_PRINT_HERE();
        return {};
    }
    SuspendAlways final_suspend() noexcept {
        TCPP_PRINT_HERE();
        return {};
    }
    void unhandled_exception() {}
    void return_value(T&& t) {
        TCPP_PRINT_HERE();
        value_ = std::move(t);
    }

    T value_;
};

Awaitable<std::string> GetString() {
    TCPP_PRINT_HERE();
    co_return std::string{"some string"};
}

Awaitable<int> GetInt0() { co_return 0; }
Awaitable<int> GetInt1() { co_return co_await GetInt0() + 1; }
Awaitable<int> GetInt2() { co_return co_await GetInt1() + 2; }
Awaitable<int> GetInt3() { co_return co_await GetInt2() + 3; }
Awaitable<int> GetInt4() { co_return co_await GetInt3() + 4; }
Awaitable<int> GetInt5() { co_return co_await GetInt4() + 5; }

Awaitable<std::string> Test0Coro() {
    TCPP_PRINT_HERE();
    auto str = co_await GetString();
    TCPP_PRINT_HERE();

    std::size_t i = 0;
    for (; i < 100000; i++) {
        // if (i % 100 == 0)
        {
            ::fmt::print("i = {}\n", i);
        }
        auto const value = co_await GetInt5();
        ::fmt::print("value = {}\n", value);
    }

    co_return std::move(str);
}

Awaitable<int> co_Test0() {
    auto str = co_await Test0Coro();
    ::fmt::print("str = {}\n", str);
    co_return 10;
}

void Test0() {
    TCPP_PRINT_HERE();
    auto root = [](auto&& awaitable) -> Awaitable<int> {
        TCPP_PRINT_HERE();
        auto str = co_await awaitable;
        TCPP_PRINT_HERE();
        ::fmt::print("str = {}\n", str);
        co_return 0;
    }(Test0Coro());
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
