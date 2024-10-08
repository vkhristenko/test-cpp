#pragma once

#include <string>

#include "core/macros.h"

namespace core {

struct SourceLocation {
    std::string file_name{};
    std::uint32_t line{0};
};

}

#define TCPP_CURRENT_SOURCE_CONTEXT() \
    ::core::SourceLocation{ .file_name{TCPP_FILE()}, .line{TCPP_LINE()}}
