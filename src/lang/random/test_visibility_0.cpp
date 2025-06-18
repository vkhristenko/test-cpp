#include <iostream>

class Base {
protected:
    virtual void SomeFunction() const {}
};

class Derived : public Base {
public:
    using Base::SomeFunction;
};

void Test0() {
    auto d = Derived{};
    d.SomeFunction();
}

int main() {
    Test0();

    return 0;
}
