#include <iostream>
#include <array>
#include <memory>

#include "fmt/core.h"

#include "core/result.h"
#include "core/macros.h"

#include "minaio/IOContext.h"

using namespace core;
using namespace minaio;

struct Connection : public std::enable_shared_from_this<Connection> {
    explicit Connection(StreamSocket s)
        : s_{std::move(s)}
    {}

    void Start() noexcept {
        TCPP_PRETTY_FUNCTION();
        DoRead();
    }

private:
    void DoRead() noexcept {
        TCPP_PRETTY_FUNCTION();
        s_.AsyncRead(
            buffer_.data(), 
            buffer_.size(),
            [self = shared_from_this(), this](Result<std::size_t, std::string> re_bytes) {
                if (re_bytes.HasError()) {
                    ::fmt::print("failed to read!");
                    return ;
                }

                ::fmt::print("read in {} bytes", re_bytes.ValueUnsafe());
                DoWrite(re_bytes.ValueUnsafe());
            }
        );
    }       

    void DoWrite(std::size_t const nbytes) noexcept {
        TCPP_PRETTY_FUNCTION(); 

        s_.AsyncWrite(
            buffer_.data(),
            nbytes,
            [self = shared_from_this(), this](Result<std::size_t, std::string> re_nbytes) {
                if (re_nbytes.HasError()) {
                    ::fmt::print("failed to write!");
                    return ;
                }

                ::fmt::print("written out {} bytes", re_nbytes.ValueUnsafe());
                DoRead();
            }
        );
    }

    StreamSocket s_;
    std::array<char, 1024> buffer_;
};

struct Server {
    explicit Server(IOContext& ctx, unsigned int const port)
        : acceptor_{ctx, Endpoint{::fmt::format(":{}", port)}}
    {
        DoAccept();
    }

private:
    void DoAccept() noexcept {
        acceptor_.AsyncAccept(
            [this](Result<StreamSocket, std::string> s) {
                if (s.HasError()) {
                    ::fmt::print("{}\n", s.ErrorUnsafe());
                    return ;
                }

                ::fmt::print("new connection!\n");
                auto conn = std::make_shared<Connection>(std::move(s).ValueUnsafe());
                conn->Start();
            }
        );
    }

    Acceptor acceptor_;
};

void Test0() {
    IOContext io_ctx{};

    Server server{io_ctx, 12345};

    // main event loop
    for (;;) {
        io_ctx.Poll();
    }
}

int main() {
    Test0();

    return 0;
}
