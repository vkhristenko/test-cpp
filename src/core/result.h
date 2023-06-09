#pragma once

#include <exception>
#include <variant>
#include <stdexcept>

namespace core {

template <typename T, typename E>
class Result {
    // type requirements
    static_assert(!std::is_same<T, E>::value, "Value/Error types must be different");

public:
    Result() = delete;
    Result(Result const&) = default;
    Result(Result&&) = default;
    Result& operator=(Result const&) = default;
    Result& operator=(Result&&) = default;

    // generalize
    Result(const T& value) 
        : data_{value}
    {}

    Result(T&& value) 
        : data_{std::move(value)}
    {}

    Result(const E& error) 
        : data_{error}
    {}
    Result(E&& error) 
        : data_{std::move(error)}
    {}

    bool HasValue() const noexcept { return data_.index() == 0; }
    bool HasError() const noexcept { return data_.index() == 1; }
    bool Ok() const noexcept { return HasValue(); }
    
    T const& ValueUnsafe() const& noexcept {
        return std::get<T>(data_);
    }

    T&& ValueUnsafe() && noexcept {
        return std::get<T>(std::move(data_));
    }

    E const& ErrorUnsafe() const& noexcept {
        return std::get<E>(data_);
    }

    E&& ErrorUnsafe() && noexcept {
        return std::get<E>(std::move(data_));
    }

    T const& Value() const& {
        if (HasError()) {
            throw std::runtime_error{"No Value"};
        }

        return ValueUnsafe();
    }

    T&& Value() && {
        if (HasError()) {
            throw std::runtime_error{"No Value"};
        }

        return std::move(*this).ValueUnsafe();
    }

    E const& Error() const& {
        if (HasValue()) {
            throw std::runtime_error{"No Error"};
        }

        return ErrorUnsafe();
    }

    E&& Error() && {
        if (HasValue()) {
            throw std::runtime_error{"No Error"};
        }

        return std::move(*this).ErrorUnsafe();
    }

private:
    std::variant<T, E> data_;
};

}  // namespace core
