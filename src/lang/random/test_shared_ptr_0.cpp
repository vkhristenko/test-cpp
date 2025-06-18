#include <iostream>

struct Foo {
    virtual ~Foo() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

struct Bar : Foo {};

struct Deleter {
    void operator()(Foo* ptr) const {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        delete ptr;
    }
};

void Test0() { std::shared_ptr<Foo> ptr{new Bar{}, Deleter{}}; }

int main() {
    Test0();

    return 0;
}
