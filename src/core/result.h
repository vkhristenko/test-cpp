#pragma once

#include <stdexcept>

namespace core {

template <typename T, typename E>
class Result {
public:
    Result(const T& value) : ok_{true}, value_{value} {}
    Result(T&& value) : ok_{true}, value_{std::move(value)} {}
    Result(const E& error) : ok_{false}, error_{error} {}
    Result(E&& error) : ok_{false}, error_{std::move(error)} {}

    bool is_ok() const noexcept { return ok_; }
    bool is_err() const noexcept { return !ok_; }

    const T& unwrap() const& noexcept(false) {
        if (ok_) {
            return value_;
        } else {
            throw std::runtime_error(
                "Called unwrap() on a Result object with an error");
        }
    }

    T& unwrap() & noexcept(false) {
        if (ok_) {
            return value_;
        } else {
            throw std::runtime_error(
                "Called unwrap() on a Result object with an error");
        }
    }

    const T&& unwrap() const&& noexcept(false) {
        if (ok_) {
            return std::move(value_);
        } else {
            throw std::runtime_error(
                "Called unwrap() on a Result object with an error");
        }
    }

    T&& unwrap() && noexcept(false) {
        if (ok_) {
            return std::move(value_);
        } else {
            throw std::runtime_error(
                "Called unwrap() on a Result object with an error");
        }
    }

    const E& unwrap_err() const& noexcept(false) {
        if (!ok_) {
            return error_;
        } else {
            throw std::runtime_error(
                "Called unwrap_err() on a Result object with a value");
        }
    }

    E& unwrap_err() & noexcept(false) {
        if (!ok_) {
            return error_;
        } else {
            throw std::runtime_error(
                "Called unwrap_err() on a Result object with a value");
        }
    }

    const E&& unwrap_err() const&& noexcept(false) {
        if (!ok_) {
            return std::move(error_);
        } else {
            throw std::runtime_error(
                "Called unwrap_err() on a Result object with a value");
        }
    }

    E&& unwrap_err() && noexcept(false) {
        if (!ok_) {
            return std::move(error_);
        } else {
            throw std::runtime_error(
                "Called unwrap_err() on a Result object with a value");
        }
    }

    const T& unwrap_unsafe() const& noexcept(true) { return value_; }

    T& unwrap_unsafe() & noexcept(true) { return value_; }

    const T&& unwrap_unsafe() const&& noexcept(true) {
        return std::move(value_);
    }

    T&& unwrap_unsafe() && noexcept(true) { return std::move(value_); }

    const E& unwrap_err_unsafe() const& noexcept(true) { return error_; }

    E& unwrap_err_unsafe() & noexcept(true) { return error_; }

    const E&& unwrap_err_unsafe() const&& noexcept(true) {
        return std::move(error_);
    }

    E&& unwrap_err_unsafe() && noexcept(true) { return std::move(error_); }

private:
    bool ok_;
    union {
        T value_;
        E error_;
    };
};

}  // namespace core
