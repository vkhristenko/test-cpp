#include "minaio/v2/datagram_socket.h"

namespace minaio::v2 {

DatagramSocket::DatagramSocket(Context& ctx, Endpoint const& local_endpoint)
    : ctx_{&ctx}, state_{std::make_unique<State>(*this)} {
    auto fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        throw TCPP_MAKE_ERROR(PosixErrorDetails{errno});
    }

    auto ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    if (ret < 0) {
        throw TCPP_MAKE_ERROR(PosixErrorDetails{errno});
    }

    sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(local_endpoint.port());
    sin.sin_addr = in_addr{INADDR_ANY};
    if (bind(fd, reinterpret_cast<sockaddr const*>(&sin), sizeof(sin)) < 0) {
        throw TCPP_MAKE_ERROR(PosixErrorDetails{errno});
    }

    fd_ = FD{fd};

    ReactorEvent e{ReactorEvent::kNone};
    SetReadEvent(e);
    SetWriteEvent(e);
    SetCloseEvent(e);
}

DatagramSocket::DatagramSocket(DatagramSocket&& rhs)
    : ctx_{rhs.ctx_}, state_{std::move(rhs.state_)} {
    if (state_) {
        state_->self_ = this;
    }
}

DatagramSocket& DatagramSocket::operator=(DatagramSocket&& rhs) {
    Close();

    state_ = std::move(rhs.state_);
    if (state_) {
        state_->self_ = this;
    }
    ctx_ = rhs.ctx_;

    return *this;
}

void DatagramSocket::Close() noexcept {
    if (state_) {
        state_->closed_ = true;
        ctx_->reactor().Remove(state_->fd_.fd());
        if (state_->rx_ch_) {
            ctx_->Enqueue([state = state_] {
                auto __ch = std::move(state->rx_ch_);
                __ch(TCPP_MAKE_ERROR(
                    NetUserErrorDetails{NetUserErrorDetails::Code::kClosedFD}));
            });
        }
        if (state_->tx_ch_) {
            ctx_->Enqueue([state = state_] {
                auto __ch = std::move(state->tx_ch_);
                __ch(TCPP_MAKE_ERROR(
                    NetUserErrorDetails{NetUserErrorDetails::Code::kClosedFD}));
            })
        }
        state_->fd_.close();
        state_ = nullptr;
    }
}

void DatagramSocket::HandleEvent(std::uint8_t event_mask) noexcept {
    if (IsReadEvent(event_mask) && state_->rx_ch_) {
        StartRead();
    }

    if (IsWriteEvent(event_mask) && state_->tx_ch_) {
        StartWrite();
    }

    if (IsCloseEvent(event_mask)) {
        Close();
        return;
    }
}

void DatagramSocket::StartRead() noexcept {
    auto [buffer, size, remote_ptr] = state_->rx_data_;

    sockaddr_in src_addr;
    socklen_t src_addr_size = sizeof(sockaddr_in);

    auto const re =
        remote_ptr
            ? recvfrom(state_->fd_->fd(), buffer, size, 0,
                       reinterpret_cast<sockaddr*>(&src_addr), &src_addr_size)
            : recvfrom(state_->fd_->fd(), buffer, size, 0, nullptr, nullptr);

    /// end of file
    if (re == 0) {
        ctx_->Enqueue([state = state_] {
            auto __ch = std::move(state->rx_ch_);
            __ch(TCPP_MAKE_ERROR(
                NetUserErrorDetails{NetUserErrorDetails::Code::kClosedFD}));
        });
        return;
    }

    /// nbytes
    if (re > 0) {
        if (remote_ptr) {
            if (src_addr_size > sizeof(sockaddr_in)) {
                *remote_ptr = Endpoint{"", 0};
            } else {
                *remote_ptr =
                    Endpoint{std::string{inet_ntoa(src_addr.sin_addr)},
                             ntohs(src_addr.sin_port), "", false};
            }
        }

        ctx_->Enqueue([re, state = state_] {
            auto __ch = std::move(state->rx_ch_);
            __ch(static_cast<std::size_t>(re));
        });
        return;
    }

    auto const code = errno;
    if (EAGAIN != code) {
        ctx_->Enqueue([code, state = state_] {
            auto __ch = std::move(state->rx_ch_);
            __ch(TCPP_MAKE_ERROR(PosixErrorDetails{code}));
        });
        return;
    }

    /// nothing to read yet
}

void DatagramSocket::StartWrite() noexcept {
    auto [buffer, size, u] = state->tx_data_;

    sockaddr_in dest_addr;
    if (!to_sockaddr(*u, dest_addr)) {
        ctx_->Enqueue([state = state_] {
            auto __ch = std::move(state->tx_ch_);
            __ch(TCPP_MAKE_ERROR(
                core::StringErrorDetails{"invalid destination address"}));
        });
        return;
    }

    auto const re = ::sendto(state_->fd_.fd(), buffer, size, 0,
                             reinterpret_cast<sockaddr const*>(&dest_addr),
                             sizeof(dest_addr));

    /// nbytes written
    if (re >= 0) {
        ctx_->Enqueue([re, state = state_] {
            auto __ch = std::move(state->tx_ch_);
            __ch(static_cast<std::size_t>(re));
        });
        return;
    }

    auto const code = errno;
    if (EAGAIN != code) {
        ctx_->Enqueue([code, state = state_] {
            auto __ch = std::move(state->tx_ch_);
            __ch(TCPP_MAKE_ERROR(PosixErrorDetails{code}));
        });
        return;
    }

    /// nothing to write yet
}

}  // namespace minaio::v2
