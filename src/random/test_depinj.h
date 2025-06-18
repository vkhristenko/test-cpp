#pragma once

#include <iostream>

#include "random/test_depinj_async_stream.h"
#include "random/test_depinj_macros.h"

struct TestDepInj {
    TCPP_INJECT_DEPENDENCY_DECLARE(AsyncStream, IAsyncStream) stream;

    void Run() {
        stream.asyncGet([]() {});
    }
};
