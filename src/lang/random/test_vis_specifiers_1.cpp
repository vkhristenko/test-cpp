#include <iostream>

struct Base {
protected:
    int a;
};

struct Derived : public Base {};

struct Derived2 : public Derived {
    using Base::a;
};

void Test0() {
    auto d = Derived2{};
    std::cout << d.a << std::endl;
}

int main() {
    Test0();
    return 0;
}
