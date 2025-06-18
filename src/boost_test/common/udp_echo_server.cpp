#include <boost/asio/executor_work_guard.hpp>
#include <iostream>
#include <memory>

#include "bext/utils/aliases.h"
#include "bext/utils/ctx.h"
#include "core/macros.h"
#include "fmt/core.h"

struct Server {
    explicit Server(::basio::io_context& ctx, unsigned int port)
        : s_{ctx, ::basio::ip::udp::endpoint(::basio::ip::udp::v4(), port)} {
        TCPP_PRINT_PRETTY_FUNCTION();
        Start();
    }

    void Start() noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        DoRead();
    }

private:
    void DoRead() noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        s_.async_receive_from(
            ::basio::buffer(buf_.data(), buf_.size()), src_,
            [this](::bsys::error_code ec, std::size_t nbytes) {
                if (!ec && nbytes > 0) {
                    DoWrite(nbytes);
                } else {
                    DoRead();
                }
            });
    }
    void DoWrite(std::size_t nbytes) noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        assert(nbytes < buf_.size() &&
               "bytes received must be less than buffer capacity!");
        s_.async_send_to(
            ::basio::buffer(buf_.data(), nbytes), src_,
            [this](::bsys::error_code ec, std::size_t) { DoRead(); });
    }

    ::basio::ip::udp::socket s_;
    std::array<char, 1024> buf_;
    ::basio::ip::udp::endpoint src_;
};

int main() {
    TCPP_PRINT_HELLO_WORLD;

    ::basio::io_context ctx;
    Server server{ctx, 12345};
    bext::utils::RunInThisThread(ctx);

    return 0;
}
