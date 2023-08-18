#pragma once

#include <string>

#include "core/macros.h"

namespace core {

struct SourceContext {
    std::string file_name{};
    std::uint32_t line{0};
};

}

#define TCPP_CURRENT_SOURCE_CONTEXT() \
    ::core::SourceContext(TCPP_FILE(), TCPP_LINE())
