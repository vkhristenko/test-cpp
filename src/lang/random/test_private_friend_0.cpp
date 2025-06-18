#include <iostream>

class A {
    int a = 10;

    friend class ATesting;
};

class B {
    A a;

    friend class BTesting;
};

class ATesting {
public:
    static auto const& Get(A const& a) { return a.a; }
};

class BTesting {
public:
    static auto const& Get(B const& b) { return b.a; }
};

void Test0() {
    std::cout << "hello world" << std::endl;

    B b;
    std::cout << ATesting::Get(BTesting::Get(b)) << std::endl;
}

int main() {
    Test0();

    return 0;
}
