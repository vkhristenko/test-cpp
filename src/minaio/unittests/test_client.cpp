#include <iostream>
#include <array>
#include <memory>

#include "fmt/core.h"

#include "core/result.h"
#include "core/macros.h"

#include "minaio/IOContext.h"

using namespace core;
using namespace minaio;                                                                 
struct Connection {
    explicit Connection(IOContext& io_ctx) 
        : io_ctx_{io_ctx} 
        , connector_{io_ctx_}
    {
        DoConnect();
    }

private:
    void DoConnect() {
        // TODO might be bad design
        connector_.AsyncConnect(
            [this](Result<StreamSocket, std::string> re) {
            }
        );
    }

private:
    IOContext& io_ctx_;
    Connector connector_;
};

void Test0() {
    IOContext io_ctx;

    Connection client{io_ctx};

    for (;;) {
        io_ctx.Poll();
    }
}

int main() {
    Test0();

    return 0;
}
