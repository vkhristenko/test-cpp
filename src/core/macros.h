#pragma once

#define TCPP_PRETTY_FUNCTION() __PRETTY_FUNCTION__
#define TCPP_FUNCTION() __FUNCTION__
#define TCPP_LINE() __LINE__
#define TCPP_FILE() __FILE__

#define TCPP_PRINT_EXPR_RESULT(EXPR) \
    ::fmt::print("( {} ) = {}\n", #EXPR, (EXPR))

#define TCPP_PRINT_HELLO_WORLD \
    ::fmt::print("hello world\n")

#define TCPP_PRINT_PRETTY_FUNCTION() \
    ::fmt::print("{}\n", TCPP_PRETTY_FUNCTION())
