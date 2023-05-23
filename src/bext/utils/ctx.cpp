#include "bext/utils/ctx.h"

#include "fmt/core.h"

namespace bext::utils {

void RunInThisThread(::basio::io_context& ctx) noexcept {
    for (;;) {
        try {
            ctx.run();
        } catch (...) {
            // TODO source location macros + error handling!
            ::fmt::print("unknown exception caught! keep going!");
        }
    }
}

}
