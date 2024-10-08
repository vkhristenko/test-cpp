#pragma once

#include <iostream>

struct AsyncStream {
    template<typename Callable>
    void asyncGet(Callable&& ch) noexcept { 
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        ch(); 
    }
};
