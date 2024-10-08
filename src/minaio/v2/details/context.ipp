#pragma once

#include "minaio/v2/context.h"
#include "core_cpp20/at_scope_exit.h"

namespace minaio::v2 {

template<typename UTTraits>
uint32_t ContextImpl<UTTraits>::Poll() {
    if (poll_in_progress_) {
        return 0;
    }
    poll_in_progress_ = true;
    TCPP_AT_SCOPE_EXIT(poll_in_progress_ = false);

    ops_.push([reactor = &reactor_] { reactor->Poll(); });

    auto const n = ops_.size();
    for (std::size_t i=0; i<n; i++) {
        auto op = std::move(ops_.front());
        ops_.pop();
        op();
    }

    return n;
}

}
