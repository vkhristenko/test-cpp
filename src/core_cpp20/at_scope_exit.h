#pragma once

namespace core::details {

template <typename F>
struct AtScopeExit {
    template <typename U>
        requires(std::is_same_v<typename std::decay_t<U>, F> &&
                 std::is_nothrow_copy_constructible_v<U> &&
                 std::is_nothrow_move_constructible_v<U>)
    AtScopeExit(U&& f) noexcept : func_{std::forward<U>(f)} {}

    ~AtScopeExit() noexcept {
        try {
            func_();
        } catch (...) {
            /// TODO could be more granular
            TCPP_ABORT("caught something in the destructor...");
        }
    }

    F func_;
};

template <typename F>
AtScopeExit(F&&) -> AtScopeExit<typename std::decay_t<F>>;

}  // namespace core::details

#define TCPP_AT_SCOPE_EXIT(...) TCPP_AT_SCOPE_EXIT_IMPL((__VA_ARGS__))
#define TCPP_AT_SCOPE_EXIT_IMPL(EXPR)                          \
    auto TCPP_UNIQUE_LOCAL_ID = ::core::details::AtScopeExit { \
        [&]() noexcept(noexcept(EXPR)) -> void { EXPR; }       \
    }
