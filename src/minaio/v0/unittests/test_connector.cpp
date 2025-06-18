#include <array>
#include <iostream>
#include <memory>

#include "core/macros.h"
#include "core/result.h"
#include "fmt/core.h"
#include "minaio/connector.h"

using namespace core;
using namespace minaio;

struct Client {
    explicit Client(IOContext& ctx, Endpoint const& e)
        : connector_{ctx}, e_{e} {
        DoConnect();
    }

private:
    void DoConnect() noexcept {
        connector_.AsyncConnect(e_, [this](
                                        Result<StreamSocket, std::string> s) {
            if (s.HasError()) {
                ::fmt::print("{}\n", s.ErrorUnsafe());
                return;
            }

            ::fmt::print("successfully conected on fd = {}\n", s.ValueUnsafe());
            std::exit(0);
        });
    }

    Connector connector_;
    Endpoint e_;
};

void Test0() {
    IOContext io_ctx{};

    Client client{io_ctx, 12345};

    // main event loop
    for (;;) {
        io_ctx.Poll();
    }
}

int main() {
    Test0();

    return 0;
}
