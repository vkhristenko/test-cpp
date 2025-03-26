#include <iostream>

struct IBase1 {
    virtual ~IBase1() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

struct IBase2 {
    virtual ~IBase2() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

struct Derived : IBase1, IBase2 {
    ~Derived() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

void Test0() {
    auto derived = new Derived{};
    auto* base1 = static_cast<IBase1*>(derived);
    delete base1;
}

int main() {
    Test0();

    return 0;
}
