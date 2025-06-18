#include <iostream>
#include <memory>

#include "bext/utils/aliases.h"

struct ClientSession : std::enable_shared_from_this<ClientSession> {
    ClientSession(::basio::io_context& ctx,
                  ::basio::ip::tcp::endpoint&& endpoint)
        : ctx_{ctx}, endpoint_{std::move(endpoint)} {}

    void start() {}

    ::basio::io_context& ctx_;
    ::basio::ip::tcp::endpoint endpoint_;
};

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "./exe address port" << std::endl;
        return 1;
    }

    auto address = ::basio::ip::make_address(argv[1]);
    auto const port = static_cast<uint16_t>(std::atoi(argv[2]));

    ::basio::io_context ctx;

    auto client_session = std::make_shared<ClientSession>(
        ctx, ::basio::ip::tcp::endpoint{address, port});
    client_session->start();

    while (1) {
        ctx.poll();
    }

    return 0;
}
