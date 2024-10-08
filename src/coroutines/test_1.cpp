#include <experimental/coroutine>
  
#include "core/macros.h"

#if 0
struct Promise;

struct AwaitableInt {
    using promise_type = ::Promise;

    AwaitableInt(std::experimental::coroutine_handle<Promise> this_handle)
        : this_handle_{std::move(this_handle)}
    {}

    bool await_ready() { return false; }
    void await_suspend(std::experimental::coroutine_handle<Promise> h) {
        prev_handle_ = std::move(h);

    }
    void await_resume() {}

    std::experimental::coroutine_handle<Promise> prev_handle_;
    std::experimental::coroutine_handle<Promise> this_handle_;
};

struct Promise {
    AwaitableInt get_return_objec(){ 
        return AwaitableInt{
            std::experimental::coroutine_handle<Promise>::from_promise(*this)
        };
    }

    std::experimental::suspend_always initial_suspend() noexcept { return {}; }
    std::experimental::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
};

AwaitableInt get_2_0() {
    co_return 100;
}

AwaitableInt get_2_1() {
    co_return 1000;
}

AwaitableInt get_2_2() {
    co_return 10000;
}

AwaitableInt get_1_0() {
    ::fmt::print("value = {}", co_await get_2_0());
    ::fmt::print("value = {}", co_await get_2_1());
    ::fmt::print("value = {}", co_await get_2_2());
}

AwaitableInt get_1_1() {
    ::fmt::print("value = {}", co_await get_2_0());
    ::fmt::print("value = {}", co_await get_2_1());
    ::fmt::print("value = {}", co_await get_2_2());
}

AwaitableInt get_1_2() {
    ::fmt::print("value = {}", co_await get_2_0());
    ::fmt::print("value = {}", co_await get_2_1());
    ::fmt::print("value = {}", co_await get_2_2());
}

AwaitableInt get_0() {
    ::fmt::print("value = {}", co_await get_1_0());
    ::fmt::print("value = {}", co_await get_1_1());
    ::fmt::print("value = {}", co_await get_1_2());
}

void Test0() {
    Context ctx;

    auto root_coro = get_0();

    ctx.Enqueue([root = std::move(root_coro)]{ root.resume(); });

    for (;;) {
        ctx.RunOne();
    }

}
#endif

int main() {
    //Test0();

    return 0;
}
