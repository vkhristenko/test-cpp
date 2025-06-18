#pragma once

#include <initializer_list>

namespace mystd {

template <typename T, std::size_t N>
class Array final {
public:
    Array() noexcept;
    Array(std::initializer_list<T>) noexcept;

    Array(Array const&) noexcept;
    Array& operator=(Array const&) noexcept;
    Array(Array&&) noexcept;
    Array& operator=(Array&&) noexcept;

    constexpr std::size_t size() noexcept { return N; }

    T const& operator[](std::size_t i) const noexcept { return data_[i]; }
    T& operator[](std::size_t i) noexcept { return data_[i]; }

    /// TODO
    /// views/ranges/iterators

private:
    T data_[N];
};

}  // namespace mystd
