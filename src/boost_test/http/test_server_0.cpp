#include <iostream>
#include <memory>

#include "bext/utils/aliases.h"

struct Listener : std::enable_shared_from_this<Listener> {
    Listener(::basio::io_context& ctx, ::basio::ip::tcp::endpoint&& endpoint)
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
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

    ::basio::io_context ctx;

    auto listener = std::make_shared<Listener>(
        ctx, ::basio::ip::tcp::endpoint{address, port});
    listener->start();

    while (true) {
        ctx.poll();
    }

    return 0;
}
