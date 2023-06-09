#pragma once

#include <string>

#include "fmt/core.h"

#include "core/macros.h"

namespace minaio {

class Endpoint {
public:
    explicit Endpoint(std::string const&) {
        TCPP_PRINT_PRETTY_FUNCTION();
    }

    unsigned int Port() const noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        return 0;
    }
};


class ExecutionContext {
public:
    virtual ~ExecutionContext() noexcept = default;

    virtual void Poll() = 0;
};

class IOContext final : public ExecutionContext {
public:
    IOContext() {
        TCPP_PRINT_PRETTY_FUNCTION();
    }

    ~IOContext() noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
    }

    void Poll() override {
        TCPP_PRINT_PRETTY_FUNCTION();
    }

public:
};

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

class Acceptor final {
public:
    Acceptor(IOContext& io_ctx, Endpoint endpoint)
        : io_ctx_{io_ctx}
        , endpoint_{std::move(endpoint)}
    {
        // create a file descriptor - non blocking one
        // Add it to the set of watched fds
        TCPP_PRINT_PRETTY_FUNCTION();
    }

    template<typename Callable>
    void AsyncAccept(Callable&&) noexcept {
        // perform accept in non-blocking fashion
        // assign a callback to run
        TCPP_PRINT_PRETTY_FUNCTION();
    }

private:
    IOContext& io_ctx_;
    Endpoint endpoint_;
};

}
