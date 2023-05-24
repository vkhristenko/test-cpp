#include <iostream>

#include "fmt/core.h"

#include <boost/asio/executor_work_guard.hpp>

#include "bext/utils/aliases.h"
#include "bext/utils/ctx.h"
#include "core/macros.h"

int main() {
    ::basio::io_context ctx;
    ::basio::ip::udp::socket s{ctx, ::basio::ip::udp::endpoint(
        ::basio::ip::udp::v4(), 0)};

    ::basio::ip::udp::resolver resolver{ctx};
    std::string msg_in = "some string!";
    auto endpoints = resolver.resolve(::basio::ip::udp::v4(), "localhost", "12345");
    s.send_to(::basio::buffer(msg_in), *endpoints.begin());
   
    std::array<char, 1024> buf;
    ::basio::ip::udp::endpoint dest;
    auto bytes = s.receive_from(
        ::basio::buffer(buf.data(), buf.size()),
        dest
    );
    std::string msg{buf.data(), bytes};
    ::fmt::print("msg sent: '{}' and msg received: '{}'", msg_in, msg);

    return 0;
}
