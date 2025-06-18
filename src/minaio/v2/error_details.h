#pragma once

#include <sstream>

#include "core/error.h"

namespace minaio::v2 {

class NetErrorDetails : public core::ErrorDetailsHelper<NetErrorDetails> {
public:
    NetErrorDetails() = default;

    std::unique_ptr<core::IErrorDetails> Clone() const noexcept override {
        return std::make_unique<NetErrorDetails>();
    }

    std::string to_string() const noexcept override {
        return "NetErrorDetails";
    }

    static void const* ClassId() noexcept { return &s_id; }

private:
    static char s_id;
};

class NetUserErrorDetails final
    : public core::ErrorDetailsHelper<NetUserErrorDetails, NetErrorDetails> {
public:
    enum class Code : std::uint8_t {
        kCancelledAsyncOp = 0,
        kEndOfFile = 1,
        kClosedFD = 2
    };

    explicit NetUserErrorDetails(Code c) noexcept : code_{c} {}

    std::unique_ptr<core::IErrorDetails> Clone() const noexcept override {
        return std::make_unique<NetUserErrorDetails>(code_);
    }

    std::string to_string() const noexcept override { return "TODO"; }

    Code code() const noexcept { return code_; }

    static void const* ClassId() noexcept { return &s_id; }

private:
    Code code_;
    static char s_id;
};

class PosixErrorDetails final
    : public core::ErrorDetailsHelper<PosixErrorDetails, NetErrorDetails> {
public:
    PosixErrorDetails(int e) : errno_{e} {}

    std::unique_ptr<core::IErrorDetails> Clone() const noexcept override {
        return std::make_unique<PosixErrorDetails>(errno_);
    }

    std::string to_string() const noexcept override {
        std::stringstream ss;
        ss << "errno: " << errno_ << ", errstr: " << strerror(errno_);
        return ss.str();
    }

    int error_number() const noexcept { return errno_; }

    std::string_view error_string_view() const noexcept {
        return strerror(errno_);
    }

    static void const* ClassId() noexcept { return &s_id; }

private:
    int errno_;
    static char s_id;
};

}  // namespace minaio::v2
