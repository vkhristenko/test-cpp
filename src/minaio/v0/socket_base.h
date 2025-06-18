#pragma once

#include <queue>
#include <string>
#include <unordered_map>

#include "core/macros.h"
#include "core/result.h"
#include "fmt/core.h"
#include "minaio/endpoint.h"

namespace minaio {

//
// encapsulates basic operations for socket file descriptor
//
class SocketBase {
public:
    explicit SocketBase(int fd) : fd_{fd} {
        TCPP_PRINT_PRETTY_FUNCTION();
        // passing a negative fd sets invalid state
        if (fd_ < 0) {
            fd_ = -1;
            return;
        }

        SetNonBlocking();
    }
    SocketBase() {
        TCPP_PRINT_PRETTY_FUNCTION();
        Create();
        SetNonBlocking();
    }
    SocketBase(SocketBase const&) = default;
    SocketBase(SocketBase&&) = default;
    SocketBase& operator=(SocketBase const&) = default;
    SocketBase& operator=(SocketBase&&) = default;
    ~SocketBase() noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        DoClose();
    }

    void SetNonBlocking() {
        TCPP_PRINT_PRETTY_FUNCTION();
        //
        // Make this file descriptor non blocking
        //
        auto ret = fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL, 0) | O_NONBLOCK);
        if (ret == -1) {
            ::fmt::print("fcntl error! error = {}\n", strerror(errno));
            fd_ = -1;
            throw std::runtime_error{"fcntl error!"};
        }
    }

    int Descriptor() const noexcept { return fd_; }

    bool IsOpen() const noexcept { return fd_ > 0; }

    void Close() noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        DoClose();
    }

protected:
    void Create() noexcept {
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd_ < 0) {
            ::fmt::print("socket error! error = {}\n", strerror(errno));
            fd_ = -1;
            throw std::runtime_error{"socket error!"};
        }
    }

    void DoClose() {
        if (IsOpen()) {
            close(fd_);
            fd_ = -1;
        }
    }

protected:
    int fd_;
};

}  // namespace minaio
