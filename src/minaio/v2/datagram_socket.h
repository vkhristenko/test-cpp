#pragma once

#include "core/error.h"

#include "minaio/v2/error_details.h"
#include "minaio/v2/context.h"
#include "minaio/v2/endpoint.h"
#include "minaio/v2/fd.h"

namespace minaio::v2 {

class DatagramSocket final {
public:
    DatagramSocket() = delete;
    DatagramSocket(Context&, Endpoint const&);
    DatagramSocket(DatagramSocket const&) = delete;
    DatagramSocket& operator=(DatagramSocket const&) = delete;
    DatagramSocket(DatagramSocket&&);
    DatagramSocket& operator=(DatagramSocket&&);
    ~DatagramSocket() noexcept { Close(); }

    template<typename Callable>
        // requires AsyncCompletionHandler<std::decay_t<Callable>, ErrorOr<std::size_t>>
    void AsyncReadFrom(void*, std::size_t, Endpoint*, Callable&&) noexcept;

    template<typename Callable>
        // requires AsyncCompletionHandler<std::decay_t<Callable>, ErrorOr<std::size_t>>
    void AsyncWriteTo(void const*, std::size_t, Endpoint const&, Callable&&) noexcept;

    void Close() noexcept;

    Context& context() noexcept;

    Endpoint const& local_endpoint() const noexcept;

private:
    Context* ctx_;

    struct State {
        explicit State(DatagramSocket&);
        void HandleEvents(int fd, ReactorEvent) noexcept;
        void StartRead(void*, std::size_t, Endpoint*) noexcept;
        void StartWrite(void const*, std::size_t, Endpoint const&) noexcept;

        std::tuple<void*, std::size_t, Endpoint*> rx_data_;
        std::tuple<void const*, std::size_t, Endpoint const*> tx_data_;
        void DoRead() noexcept;
        void DoWrite() noexcept;
        DatagramSocket* self_;
    };
    std::unique_ptr<State> state_;

    FD fd_;
    core::SmallFunction<void(core::ErrorOr<std::size_t>)> rx_ch_;
    core::SmallFunction<void(core::ErrorOr<std::size_t>)> tx_ch_;
};

template<typename Callable>
void DatagramSocket::AsyncReadFrom(void* buffer, std::size_t size, Endpoint* e, Callable&& ch) noexcept {
    if (!state_) {
        ctx_->Enqueue([ch = std::forward<Callable>(ch)]{
            ch(TCPP_MAKE_ERROR(NetUserErrorDetails{NetUserErrorDetails::Code::kClosedFD}));
        });
        return ;
    }

    rx_ch_ = std::forward<Callable>(ch);
    state_->StartRead(buffer, size, e); 
}

template<typename Callable>
void DatagramSocket::AsyncWriteTo(
        void const* buffer, 
        std::size_t size, 
        Endpoint const& u,
        Callable&& ch) noexcept {
    if (!state_) {
        ctx_->Enqueue([ch = std::forward<Callable>(ch)]{
            ch(TCPP_MAKE_ERROR(NetUserErrorDetails{NetUserErrorDetails::Code::kClosedFD}));
        });
        return ;
    }

    tx_ch_ = std::forward<Callable>(ch);
    state_->StartWrite(buffer, size, u);
}

}
