#pragma once

#include <unordered_map>
#include <queue>
#include <string>

#include "fmt/core.h"

#include "core/result.h"
#include "core/macros.h"

#include "minaio/endpoint.h"
#include "minaio/socket_base.h"
#include "minaio/io_context.h"
#include "minaio/stream_socket.h"

namespace minaio {

class Acceptor;

class AcceptorReactor : public EventHandler {
public:
    AcceptorReactor(Acceptor& a) : a_{a} {
        TCPP_PRINT_PRETTY_FUNCTION();
    }

    void Handle(epoll_event const&) noexcept override;

    void Destroy() {
        TCPP_PRINT_PRETTY_FUNCTION();
        delete this;
    }

    core::Result<SocketBase, std::string> DoAsyncAcceptInitiation() noexcept;

private:
    friend class Acceptor;
    Acceptor& a_;
    std::function<void(core::Result<SocketBase, std::string>)> ch_;
};

class Acceptor final : protected SocketBase {
public:
    Acceptor(IOContext& io_ctx, Endpoint endpoint)
        : SocketBase()
        , io_ctx_{io_ctx}
        , endpoint_{std::move(endpoint)}
    {
        TCPP_PRINT_PRETTY_FUNCTION();

        // 
        // make sure we have a valid fd
        //
        if (!IsOpen()) {
            throw std::runtime_error{"accepting socket is not opened!"};
        }

        //
        // Prepare this socket for accepting connections
        //
        sockaddr_in sin{};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(endpoint_.port());
        sin.sin_addr = in_addr{0};
        if (bind(fd_, reinterpret_cast<sockaddr const*>(&sin), sizeof(sin)) < 0) {
            ::fmt::print("bind error: {}\n", strerror(errno));
            throw std::runtime_error{"bind error"};
        }

        if (listen(fd_, 10) < 0) {
            ::fmt::print("listen error {}", strerror(errno));
            throw std::runtime_error{"listen error!"};
        }

        //
        // create a reactor and register it with io context
        //
        reactor_ = new AcceptorReactor{*this};
        epoll_event eevent;
        eevent.events = EPOLLIN;
        eevent.data.fd = fd_;
        auto re = reactor_->Register(io_ctx_, fd_, eevent);
        if (re.HasError()) {
            throw std::runtime_error{re.ErrorUnsafe()};
        }
    }

    ~Acceptor() noexcept {
        DoCloseSuper();
    }

    template<typename Callable>
    // Callable = void(Result<StreamSocket, std::string>)
    void AsyncAccept(Callable&& callable) noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();

        // 
        // completion handler implies async op in progress
        //
        reactor_->ch_ = std::move(callable);

        //
        // TODO being lazy... but is ok!
        //
        reactor_->Handle(epoll_event{});
    }

    void Close() noexcept {
        DoCloseSuper();
        SocketBase::Close();
    }

private:
    void DoCloseSuper() noexcept {
        if (!IsOpen()) {
            return ;
        }

        reactor_->Deregister(io_ctx_, fd_);
        reactor_->Destroy();
        reactor_ = nullptr;
    }

private:
    friend AcceptorReactor;
    AcceptorReactor* reactor_;
    IOContext& io_ctx_;
    Endpoint endpoint_;
};

void AcceptorReactor::Handle(epoll_event const&) noexcept {
    TCPP_PRINT_PRETTY_FUNCTION();
    //
    // if no async op was enqueued - skip this event
    //
    if (!ch_) {
        return ;
    }

    auto re = DoAsyncAcceptInitiation();

    // 
    // if initiation completed
    //
    if (re.HasError() || (re.HasValue() && re.ValueUnsafe().IsOpen())) {
        TCPP_PRINT_HERE();
        auto cb = [re = std::move(re), ch = std::move(ch_)]() {
            ch(std::move(re));
        };
        EnqueueCompletionHandler(a_.io_ctx_, std::move(cb));
        ch_ = nullptr;
        return ;
    }

    //
    // if initiation did not complete, wait again...
    //
}
    
core::Result<SocketBase, std::string> AcceptorReactor::DoAsyncAcceptInitiation() noexcept {
    TCPP_PRINT_PRETTY_FUNCTION();
    auto client_sock = accept(a_.fd_, nullptr, nullptr);
    if (client_sock <= -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK){
            ::fmt::print("accept failed {}.\n", strerror(errno));
            return std::string{strerror(errno)};
        }

        return SocketBase{-1};
    }

    return SocketBase{client_sock};
};

}
