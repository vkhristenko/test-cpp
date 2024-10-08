#pragma once

#include "core/error.h"

#include <exception>
#include <type_traits>

namespace core {

namespace details {

template<typename F>
auto make_noexcept_impl(F&& f) noexcept -> ErrorOr<typename std::invoke_result_t<F>>
{
    using ReturnType = typename std::invoke_result_t<F>;

    try {
        if constexpr (std::is_same<ReturnType, void>::value) {
            std::forward<F>(f)();
            return {};
        } else {
            return std::forward<F>(f)();
        }
    } catch (std::exception& e) {
        return TCPP_MAKE_ERROR(StringErrorDetails{e.what()});
    } catch (...) {
        return TCPP_MAKE_ERROR(StringErrorDetails{"unknown exception"});
    }
}

}

}

#define TCPP_MAKE_NOEXCEPT(...) TCPP_MAKE_NOEXCEPT_IMPL((__VA_ARGS__))
#define TCPP_MAKE_NOEXCEPT_IMPL(EXPR) \
    [&]() noexcept -> ::core::ErrorOr<typename std::decay_t<decltype(EXPR)>> \
    { \
        auto call = [&]() -> typename std::decay_t<decltype(EXPR)> { \
            return EXPR; \
        }; \
        return ::core::details::make_noexcept_impl(std::move(call)); \
    }()
