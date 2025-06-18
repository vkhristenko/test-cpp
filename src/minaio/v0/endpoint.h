#pragma once

#include <queue>
#include <string>
#include <unordered_map>

#include "core/macros.h"
#include "core/result.h"
#include "fmt/core.h"

namespace minaio {

class Endpoint {
public:
    Endpoint() noexcept = delete;
    explicit Endpoint(std::string const&, unsigned int port) {
        TCPP_PRINT_PRETTY_FUNCTION();
    }
    ~Endpoint() noexcept = default;

    unsigned int port() const noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        return port_;
    }

protected:
    unsigned int port_;
};

}  // namespace minaio
