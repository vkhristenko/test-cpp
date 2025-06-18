#pragma once

#include <queue>
#include <string>
#include <unordered_map>

#include "core/macros.h"
#include "core/result.h"
#include "fmt/core.h"
#include "minaio/endpoint.h"
#include "minaio/io_context.h"
#include "minaio/socket_base.h"

namespace minaio {

//
// TODO...
// a stream to represent a tcp-based socket comm
// TODO may be we do need Connector in here instead of keeping track if we are
// server or client side stream
//
class StreamSocket final : protected SocketBase {
public:
    template <typename Callable>
    // Callable = void(REsult<std::size, std::string>)
    void AsyncRead(char* data, std::size_t const max_size,
                   Callable&&) noexcept {
        // perform a read in non-blocking fashion
        // assign a callback to run on copmletion
        TCPP_PRINT_PRETTY_FUNCTION();
    }

    template <typename Callable>
    // Callable = void(Result<std::size, std::string>)
    void AsyncWrite(char const* data, std::size_t const max_size,
                    Callable&&) noexcept {
        // perform a write in non-blockin fashion
        // if can not write, assign initiation
        // assign a callback to run on completion
        TCPP_PRINT_PRETTY_FUNCTION();
    }

protected:
    //
    // disallow creation of socket from user side
    //
    StreamSocket(IOContext& io_ctx, int fd) : SocketBase(fd) {
        TCPP_PRINT_PRETTY_FUNCTION();
        if (!IsOpen()) {
            throw std::runtime_error{"invalid file descriptor"};
        }
    }

protected:
    IOContext* io_ctx_;

    friend class Acceptor;
    friend class Connector;
};

}  // namespace minaio
