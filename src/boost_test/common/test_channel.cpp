#include <boost/asio/use_awaitable.hpp>
#include <iostream>

#include "bext/utils/aliases.h"
#include "bext/utils/channel.h"
#include "bext/utils/ctx.h"
#include "boost/asio/experimental/awaitable_operators.hpp"
#include "core/macros.h"
#include "core/range.h"
#include "fmt/core.h"

using namespace ::boost::asio::experimental::awaitable_operators;

::basio::awaitable<void> Produce(bext::utils::Channel<std::string>& ch, int n) {
    for (auto i : core::Range{n}) {
        co_await ch.async_send(::bsys::error_code{},
                               std::string{"value = "} + std::to_string(i),
                               ::basio::use_awaitable);
        ::fmt::print("sent: {}", std::string{"value = "} + std::to_string(i));
    }
}

::basio::awaitable<void> Consume(bext::utils::Channel<std::string>& ch, int n) {
    for (auto _ : core::Range{n}) {
        auto s = co_await ch.async_receive(::basio::use_awaitable);
        ::fmt::print("received: {}", s);
    }
}

::basio::awaitable<void> Test0(::basio::io_context& ctx) {
    TCPP_PRETTY_FUNCTION();

    bext::utils::Channel<std::string> ch{ctx};
    co_await (Consume(ch, 100) && Produce(ch, 100));

    co_return;
}

int main() {
    TCPP_PRINT_HELLO_WORLD;

    ::basio::io_context ctx;
    ::basio::co_spawn(ctx, Test0(ctx), ::basio::detached);
    ctx.run();

    return 0;
}
