#include <boost/asio/executor_work_guard.hpp>
#include <iostream>
#include <memory>

#include "bext/utils/aliases.h"
#include "bext/utils/ctx.h"
#include "core/macros.h"
#include "fmt/core.h"

struct Connection : public std::enable_shared_from_this<Connection> {
    explicit Connection(::basio::ip::tcp::socket s) : s_{std::move(s)} {}

    void Start() noexcept {
        TCPP_PRETTY_FUNCTION();
        DoRead();
    }

private:
    void DoRead() noexcept {
        TCPP_PRETTY_FUNCTION();
        ::basio::async_read_until(
            s_, buf_, '\n',
            [self = shared_from_this(), this](::bsys::error_code ec,
                                              std::size_t bytes) {
                ::fmt::print("read {} bytes\n", bytes);
                if (!ec) {
                    DoWrite();
                }
            });
    }
    void DoWrite() noexcept {
        TCPP_PRETTY_FUNCTION();
        // TODO this should be more efficient!
        std::istream _is{&buf_};
        std::string _tmp;
        _is >> _tmp;

        ::basio::async_write(s_, ::basio::buffer(_tmp + '\n'),
                             [self = shared_from_this(), this](
                                 ::bsys::error_code ec, std::size_t bytes) {
                                 ::fmt::print("wrote {} bytes\n", bytes);
                                 if (!ec) {
                                     DoRead();
                                 }
                             });
    }

    ::basio::ip::tcp::socket s_;
    ::basio::streambuf buf_;
};

struct Server {
    explicit Server(::basio::io_context& ctx, unsigned int const port)
        : acceptor_{ctx,
                    ::basio::ip::tcp::endpoint(::basio::ip::tcp::v4(), port)} {
        DoAccept();
    }

private:
    void DoAccept() noexcept {
        acceptor_.async_accept(
            [this](::bsys::error_code ec, ::basio::ip::tcp::socket s) {
                if (!ec) {
                    auto conn = std::make_shared<Connection>(std::move(s));
                    conn->Start();
                }
            });
    }

    ::basio::ip::tcp::acceptor acceptor_;
};

int main() {
    TCPP_PRINT_HELLO_WORLD;

    ::basio::io_context ctx;
    auto ctx_guard = ::basio::make_work_guard(ctx);
    Server server{ctx, 12345};
    bext::utils::RunInThisThread(ctx);

    return 0;
}
