#pragma once

#include <unordered_map>
#include <queue>
#include <string>

#include "fmt/core.h"

#include "core/result.h"
#include "core/macros.h"

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

}
