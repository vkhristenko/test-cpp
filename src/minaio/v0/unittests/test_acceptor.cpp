#include <array>
#include <iostream>
#include <memory>

#include "core/macros.h"
#include "core/result.h"
#include "fmt/core.h"
#include "minaio/acceptor.h"
#include "minaio/io_context.h"

using namespace core;
using namespace minaio;

struct Server {
    explicit Server(IOContext& ctx, unsigned int const port)
        : acceptor_{ctx, Endpoint{"", port}} {
        DoAccept();
    }

private:
    void DoAccept() noexcept {
        acceptor_.AsyncAccept([this](Result<StreamSocket, std::string> s) {
            if (s.HasError()) {
                ::fmt::print("{}\n", s.ErrorUnsafe());
                return;
            }

            count_++;
            ::fmt::print("new connection! fd = {}\n", s.ValueUnsafe());
            if (count_ == 5) {
                std::exit(0);
            }

            DoAccept();
        });
    }

    Acceptor acceptor_;
    int count_ = 0;
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
