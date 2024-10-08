#include <iostream>

#define TCPP_INJECT_DEPENDENCY(INTERFACE) \
    TCPP_INJECT_DEPENDENCY_##INTERFACE

struct Underlying {
    Underlying() {}

    void DoThis() { }

    void DoThat() {}
};
#define TCPP_INJECT_DEPENDENCY_Underlying Underlying

struct UnderlyingMock {
    UnderlyingMock() {}

    void DoThis() {}

    void DoThat() {}
};
//#define TCPP_INJECT_DEPENDENCY_Underlying UnderlyingMock

struct Something {
    Something() {}

    void DoThis() { under_.DoThis(); }

    void DoThat() { under_.DoThat(); }

    //
    // if 
    //
    TCPP_INJECT_DEPENDENCY(Underlying) under_;
};

void Test0() {
}

int main() {
    Test0();

    return 0;
}
