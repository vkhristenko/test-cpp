#pragma once

#include <unordered_map>
#include <queue>
#include <string>

#include "fmt/core.h"

#include "core/result.h"
#include "core/macros.h"

#include "minaio/common.h"

namespace minaio {

class ISocket {
public:
    virtual ~ISocket() noexcept = default;
    
    Endpoint const& LocalEndpoint() const noexcept {
        return local_endpoint_;
    }

protected:
    explicit ISocket(Endpoint const& e) 
        : local_endpoint_{e}
    {}

protected:
    Endpoint local_endpoint_;
};

class Acceptor;
class Connector;

class StreamSocket : public ISocket {
public:
    template<typename Callable>
    void AsyncRead(
            char* data,
            std::size_t const max_size,
            Callable&&) noexcept {
        // perform a read in non-blocking fashion
        // assign a callback to run on copmletion
        TCPP_PRINT_PRETTY_FUNCTION();
    }

    template<typename Callable>
    void AsyncWrite(
            char const* data,
            std::size_t const max_size,
            Callable&&) noexcept {
        // perform a write in non-blockin fashion
        // if can not write, assign initiation
        // assign a callback to run on completion
        TCPP_PRINT_PRETTY_FUNCTION();
    }
    
protected:
    StreamSocket(IOContext& ctx, Endpoint const& e)
        : ISocket(e)
        , io_ctx_{ctx}
    {
        TCPP_PRINT_PRETTY_FUNCTION();
    }

protected:
    friend class Acceptor;
    friend class Connector;
    IOContext& io_ctx_;
};

class Connector final {
public:
    Connector(IOContext& ctx) 
        : ctx_{ctx}
    {}

    template<typename Callable>
    void AsyncConnect(Callable&&) {
        // TODO
    }

private:
    IOContext& ctx_;
};

}
