#pragma once

#include <exception>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>

namespace core {

// Result<T, E> is a type that holds either a value of type T or an error of
// type E. Provides monadic error handling similar to Rust's Result or
// std::expected (C++23).
template <typename T, typename E>
class Result {
    // type requirements
    static_assert(!std::is_same<T, E>::value,
                  "Value/Error types must be different");

public:
    Result() = delete;
    Result(Result const&) = default;
    Result(Result&&) = default;
    Result& operator=(Result const&) = default;
    Result& operator=(Result&&) = default;

    // Construct from value
    Result(const T& value) : data_{value} {}
    Result(T&& value) : data_{std::move(value)} {}
    // Construct from error
    Result(const E& error) : data_{error} {}
    Result(E&& error) : data_{std::move(error)} {}

    template<typename U>
        requires (std::is_convertible_v<U, T> && ! std::is_convertible_v<U,E>)
    Result(U&& value)
        : data_{std::in_place_type_t<T>{}, std::forward<U>(value)} {}

    template<typename U>
        requires (std::is_convertible_v<U, E> && ! std::is_convertible_v<U, T>)
    Result(U&& error)
        : data_{std::in_place_type_t<E>{}, std::forward<U>(error)} {}

    // Returns true if the result contains a value
    [[nodiscard]] bool HasValue() const noexcept { return data_.index() == 0; }
    // Returns true if the result contains an error
    [[nodiscard]] bool HasError() const noexcept { return data_.index() == 1; }
    // Alias for HasValue
    [[nodiscard]] bool Ok() const noexcept { return HasValue(); }

    // Access the value without checking (unsafe)
    [[nodiscard]] T const& ValueUnsafe() const& noexcept {
        return std::get<T>(data_);
    }
    [[nodiscard]] T&& ValueUnsafe() && noexcept {
        return std::get<T>(std::move(data_));
    }
    // Access the error without checking (unsafe)
    [[nodiscard]] E const& ErrorUnsafe() const& noexcept {
        return std::get<E>(data_);
    }
    [[nodiscard]] E&& ErrorUnsafe() && noexcept {
        return std::get<E>(std::move(data_));
    }

    // Access the value, throws if error is present
    [[nodiscard]] T const& Value() const& {
        if (HasError()) {
            throw std::logic_error{"Result: No Value present"};
        }
        return ValueUnsafe();
    }
    [[nodiscard]] T&& Value() && {
        if (HasError()) {
            throw std::logic_error{"Result: No Value present"};
        }
        return std::move(*this).ValueUnsafe();
    }
    // Access the error, throws if value is present
    [[nodiscard]] E const& Error() const& {
        if (HasValue()) {
            throw std::logic_error{"Result: No Error present"};
        }
        return ErrorUnsafe();
    }
    [[nodiscard]] E&& Error() && {
        if (HasValue()) {
            throw std::logic_error{"Result: No Error present"};
        }
        return std::move(*this).ErrorUnsafe();
    }

private:
    std::variant<T, E> data_;
};

// Specialization for Result<T, void>: represents a value or nothing (no error
// type)
template <typename T>
class Result<T, void> {
public:
    Result() = default;
    Result(Result const&) = default;
    Result(Result&&) = default;
    Result& operator=(Result const&) = default;
    Result& operator=(Result&&) = default;

    // Construct from value
    explicit Result(T const& e) : data_{e} {}
    explicit Result(T&& e) : data_{std::move(e)} {}

    // Returns true if the result contains a value
    [[nodiscard]] bool HasValue() const noexcept { return !HasError(); }
    // Returns true if the result is in error state (no value)
    [[nodiscard]] bool HasError() const noexcept { return !data_.has_value(); }
    // Alias for HasValue
    [[nodiscard]] bool Ok() const noexcept { return HasValue(); }

    // Access the value without checking (unsafe)
    [[nodiscard]] T const& ValueUnsafe() const& noexcept {
        return data_.value();
    }
    [[nodiscard]] T&& ValueUnsafe() && noexcept {
        return std::move(data_).value();
    }

    // Access the value, throws if error
    [[nodiscard]] T const& Value() const& {
        if (HasError()) {
            throw std::logic_error{"Result: No Value present"};
        }
        return ValueUnsafe();
    }
    [[nodiscard]] T&& Value() && {
        if (HasError()) {
            throw std::logic_error{"Result: No Value present"};
        }
        return std::move(*this).ValueUnsafe();
    }

private:
    std::optional<T> data_;
};

// Specialization for Result<void, E>: represents an error or nothing (no value
// type)
template <typename E>
class Result<void, E> {
public:
    Result() = default;
    Result(Result const&) = default;
    Result(Result&&) = default;
    Result& operator=(Result const&) = default;
    Result& operator=(Result&&) = default;

    // Construct from error
    explicit Result(E const& e) : data_{e} {}
    explicit Result(E&& e) : data_{std::move(e)} {}

    // Returns true if the result is in value state (no error)
    [[nodiscard]] bool HasValue() const noexcept { return !HasError(); }
    // Returns true if the result contains an error
    [[nodiscard]] bool HasError() const noexcept { return data_.has_value(); }
    // Alias for HasValue
    [[nodiscard]] bool Ok() const noexcept { return HasValue(); }

    // Access the error without checking (unsafe)
    [[nodiscard]] E const& ErrorUnsafe() const& noexcept {
        return data_.value();
    }
    [[nodiscard]] E&& ErrorUnsafe() && noexcept {
        return std::move(data_).value();
    }

    // Access the error, throws if value is present
    [[nodiscard]] E const& Error() const& {
        if (HasValue()) {
            throw std::logic_error{"Result: No Error present"};
        }
        return ErrorUnsafe();
    }
    [[nodiscard]] E&& Error() && {
        if (HasValue()) {
            throw std::logic_error{"Result: No Error present"};
        }
        return std::move(*this).ErrorUnsafe();
    }

private:
    std::optional<E> data_;
};

}  // namespace core
