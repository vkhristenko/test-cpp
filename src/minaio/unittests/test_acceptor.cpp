#include <iostream>
#include <array>
#include <memory>

#include "fmt/core.h"

#include "core/result.h"
#include "core/macros.h"

#include "minaio/io_context.h"
#include "minaio/acceptor.h"

using namespace core;
using namespace minaio;

struct Server {
    explicit Server(IOContext& ctx, unsigned int const port)
        : acceptor_{ctx, Endpoint{"", port}}
    {
        DoAccept();
    }

private:
    void DoAccept() noexcept {
        acceptor_.AsyncAccept(
            [this](Result<int, std::string> s) {
                if (s.HasError()) {
                    ::fmt::print("{}\n", s.ErrorUnsafe());
                    return ;
                }

                ::fmt::print("new connection! fd = {}\n", s.ValueUnsafe());
                std::exit(0);
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
