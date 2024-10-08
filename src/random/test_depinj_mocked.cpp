#include <iostream>

struct IAsyncStream {
    template<typename Callable>
    void asyncGet(Callable&& ch) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        ch();
    }
};
#define TCPP_INJECT_DEPENDENCY_DECLARE(_, MOCK) MOCK
#include "random/test_depinj.h"

void Test0() {
    TestDepInj test{};
    test.Run();
}

int main() {
    Test0();

    return 0;
}
