#pragma once

#include "fmt/core.h"

#define TCPP_PRETTY_FUNCTION() __PRETTY_FUNCTION__
#define TCPP_FUNCTION() __FUNCTION__
#define TCPP_LINE() __LINE__
#define TCPP_FILE() __FILE__

#define TCPP_PRINT_EXPR_RESULT(EXPR) \
    ::fmt::print("( {} ) = {}\n", #EXPR, (EXPR))

#define TCPP_PRINT_HELLO_WORLD ::fmt::print("hello world\n")

#define TCPP_PRINT_HERE() ::fmt::print("{}:{}\n", TCPP_FILE(), TCPP_LINE())

#define TCPP_PRINT_PRETTY_FUNCTION() \
    ::fmt::print("{}\n", TCPP_PRETTY_FUNCTION())

#define TCPP_ABORT(MSG) \
    ::fmt::print("abort triggered @{}:{} msg:{}", TCPP_FILE(), TCPP_LINE(), MSG)

#define TCPP_ABORT_IF(EXPR, MSG) \
    if (EXPR) {                  \
        TCPP_ABORT(MSG);         \
    }                            \
    static_assert(true, "")

#define TCPP_CONCAT_IMPL(x, y) x##y
#define TCPP_CONCAT(x, y) TCPP_CONCAT_IMPL(x, y)
#define TCPP_UNIQUE_LOCAL_ID TCPP_CONCAT(__unique_local_id_, TCPP_LINE())
