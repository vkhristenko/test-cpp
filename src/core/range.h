#pragma once

#include <type_traits>

namespace core {

template <typename T,
          typename = typename std::enable_if<std::is_integral<T>::value>::type>
class Range {
public:
    class iterator {
    public:
        iterator(T value, T step) : value_(value), step_(step) {}
        T operator*() const { return value_; }
        void operator++() { value_ += step_; }
        bool operator!=(const iterator &other) const {
            if (step_ > 0) {
                return value_ < other.value_;
            } else {
                return value_ > other.value_;
            }
        }

    private:
        T value_;
        T step_;
    };

    Range(T begin, T end, T step = 1) : begin_(begin), end_(end), step_(step) {}
    Range(T end) : Range(T{0}, end) {}

    iterator begin() const { return iterator(begin_, step_); }
    iterator end() const { return iterator(end_, step_); }

private:
    T begin_;
    T end_;
    T step_;
};

template <typename T>
Range(T) -> Range<T>;

}  // namespace core
