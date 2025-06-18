#pragma once

#include <queue>

#include "core/small_function.h"
#include "minaio/v2/details/context_default_ut_traits.h"
#include "minaio/v2/reactor.h"

namespace minaio {
namespace v2 {

template <typename UTTraits = details::ContextImplDefaultUTTraits>
class ContextImpl {
public:
    ContextImpl() = default;
    ContextImpl(ContextImpl const&) = delete;
    ContextImpl& operator=(ContextImpl const&) = delete;
    ContextImpl(ContextImpl&&) = delete;
    ContextImpl& operator=(ContextImpl&&) = delete;

    template <typename Callable>
    // requires AsyncCompletionHandler<std::decay_t<Callable>>
    void Enqueue(Callable&& c) noexcept {
        ops_.push(std::forward<Callable>(c));
    }

    uint32_t Poll();

    void Stop() noexcept { stop_requested_ = true; }

    bool IsStopped() const noexcept {
        return ops_.size() == 0 && stop_requested_;
    }

    Reactor& reactor() noexcept { return reactor_; }

private:
    typename UTTraits::ReactorType reactor_;

    constexpr static auto kAuxSize =
        sizeof(core::SmallFunction<void()>) -
        core::SmallFunction<void()>::GetMaxObjectSize();
    constexpr static auto kMaxObjectSize =
        2 * sizeof(core::SmallFunction<void()>) - kAuxSize;
    std::queue<core::SmallFunction<void(), kMaxObjectSize>> ops_;
    bool poll_in_progress_{false};
    bool stop_requested_{false};
};

/// user facing explicit template decl
extern template class ContextImpl<details::ContextImplDefaultUTTraits>;

/// allows the usage of forwrad decl which is not possible if doing type
/// aliasing
struct Context final : ContextImpl<details::ContextImplDefaultUTTraits> {
    using ContextImpl<details::ContextImplDefaultUTTraits>::ContextImpl;
};

}  // namespace v2
}  // namespace minaio
