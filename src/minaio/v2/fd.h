#pragma once

#include <unistd.h>

#include "core/error.h"

namespace minaio::v2 {

class FD final {
public:
    constexpr FD() noexcept = default;
    constexpr explicit FD(int fd) noexcept
        : fd_{fd}
    {}
    constexpr FD(FD&& rhs) noexcept
        : fd_{rhs.release()}
    {}
    constexpr FD& operator=(FD&& rhs) noexcept {
        close();
        fd_ = rhs.release();
        return *this;
    }

    FD(FD const&) noexcept = delete;
    FD& operator=(FD const&) noexcept = delete;
    ~FD() noexcept { close(); }

    constexpr int fd() const noexcept { return fd_; }
    void close() noexcept  {
        core::ErrorOr<void> e {};
        
        this->close(e);
        TCPP_ABORT_IF(!e.ok(), e.ErrorUnsafe().to_string());
    }

    void close(core::ErrorOr<void>& e) noexcept {
        if (is_open()) {
            auto const re = ::close(fd_);

            if (re < 0) {
                e = TCPP_MAKE_ERROR(PosixErrorDetails{errno});
            }

            fd_ = kInvalid;
        }
    }

    constexpr bool is_open() const noexcept { return fd_ >= 0; }
    constexpr int release() noexcept {
        auto tmp = fd_;
        fd_ = kInvalid;
        return tmp;
    }

private:
    int fd_ {kInvalid};

    constexpr static int kInvalid = -1;
};

}
