#pragma once

#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "core/macros.h"
#include "core/result.h"
#include "fmt/core.h"
#include "minaio/endpoint.h"
#include "minaio/io_context.h"
#include "minaio/socket_base.h"

namespace minaio {

class Connector;

class ConnectorReactor : public EventHandler {
public:
    ConnectorReactor(Connector& c) : c_{&c} { TCPP_PRINT_PRETTY_FUNCTION(); }

    void Handle(epoll_event const&) noexcept override;

    void Destroy() {
        TCPP_PRINT_PRETTY_FUNCTION();
        delete this;
    }

    core::Result<SocketBase, std::string> DoAsyncConnectInitiation() noexcept;

private:
    friend class Connector;
    Connector* c_;

    std::optional<Endpoint> endpoint_;
    std::function<void(core::Result<SocketBase, std::string>)> ch_;
};

//
// TODO
//
// - Register sock for writability
// - attemp connection upon AsyncConnect
// - if failed, just wait for events to come in
//
class Connector final : public SocketBase {
public:
    explicit Connector(IOContext& io_ctx)
        : SocketBase(),
          io_ctx_{&io_ctx},
          reactor_{new ConnectorReactor{*this}} {
        TCPP_PRINT_PRETTY_FUNCTION();

        if (!IsOpen()) {
            throw std::runtime_error{"failed to open connector"};
        }

        epoll_event e;
        e.events = EPOLLOUT;
        e.data.fd = fd_;
        auto re = reactor_->Register(io_ctx_, fd_, e);
        if (re.HasError()) {
            throw std::runtime_error{re.ErrorUnsafe()};
        }
    }

    ~Connector() noexcept { DoCloseSuper(); }

    template <typename Callable>
    // Callable = void(Result<StreamSocket, std::string>)
    void AsyncConnect(Endpoint const& e, Callable&& callable) noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();

        //
        //
        //
        reactor_->ch_ = std::move(callable);
        reactor_->endpoint_ = e;

        //
        //
        //
        reactor_->Handle(epoll_event{0, {0}});
    }

    void Close() noexcept {
        DoCloseSuper();
        SocketBase::Close();
    }

private:
    void DoCloseSuper() noexcept {
        if (!IsOpen()) {
            return;
        }

        reactor_->Deregister(io_ctx_, fd_);
        reactor_->Destroy();
        reactor_ = nullptr;
    }

private:
    IOContext* io_ctx_;
    friend ConnectorReactor;
    ConnectorReactor* reactor_;
};

void ConnectorReactor::Handle(epoll_event const& e) noexcept {
    TCPP_PRINT_PRETTY_FUNCTION();

    if (!ch_) {
        return;
    }

    //
    // ready for completion of connect (upon failure or success)
    //
    if (e.events & EPOLLOUT) {
        //
        // check failure/success of non-blocking connect;
        //
        int result;
        auto r =
            getsockopt(c_->fd_, SOL_SOCKET, SO_ERROR, &result, sizeof(result));
        if (r < 0) {
        }

        if (re.HasError() || (re.HasValue() && re.ValueUnsafe().IsOpen())) {
            TCPP_PRINT_HERE();
            auto cb = [re = std::move(re), ch = std::move(ch_)]() {
                ch(std::move(re));
            };
            EnqueueCompletionHandler(*c_->io_ctx_, std::move(cb));
            ch_ = nullptr;
            endpoint_ = std::nullopt;
            return;
        }

        return;
    }

    //
    // TODO
    //
    // there is an error with this fd
    // - enqueue completion handler with error
    // - close the connector, which in turn
    //
    // if (e.events & (EPOLLERR | EPOLLHUP)) {
    //    auto cb = [ch = std::move(ch_)]() {
    //        ch(core::Result<StreamSocket, std::string>{"EPOLLERR or
    //        EPOLLHUP"});
    //    };
    //    EnqueueCompletionHandler(*c_->io_ctx_, std::move(cb));
    //}
}

core::Result<StreamSocket, std::string>
ConnectorReactor::DoAsyncConnectInitiation() noexcept {
    TCPP_PRINT_PRETTY_FUNCTION();

    //
    // 1) try connecting
    // 2) if in progress, return invalid stream socket
    // 3) if error, return error
    // 4) if success, return a valid stream socket
    //
}

}  // namespace minaio
