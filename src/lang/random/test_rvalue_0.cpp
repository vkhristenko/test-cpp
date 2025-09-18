#include <iostream>
#include <type_traits>

struct Test {
    Test() { std::cout << "Default constructor\n"; }
    Test(const Test&) { std::cout << "Copy constructor\n"; }
    Test(Test&&) noexcept { std::cout << "Move constructor\n"; }
    ~Test() { std::cout << "Destructor\n"; }
};

template<typename T>
std::remove_reference_t<T>&& Move(T&& arg) {
    return static_cast<std::remove_reference_t<T>&&>(arg);
}

void SomeOtherFunc(Test& arg) {
    std::cout << "Lvalue reference overload\n";
}

void SomeOtherFunc(Test&& arg) {
    std::cout << "Rvalue reference overload\n";
}

template<typename T>
T&& Forward(std::remove_reference_t<T>& arg) {
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return static_cast<T&&>(arg);
}

template<typename T>
T&& Forward(std::remove_reference_t<T>& arg) {
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return static_cast<T&&>(arg);
}

template<typename T>
void SomeFunc(T&& arg) {
    std::cout << __PRETTY_FUNCTION__ << "\n";
    SomeOtherFunc(Forward<T>(arg));
}

void TestBinding(Test& arg) {
    std::cout << "Lvalue reference binding\n";
}

void TestBinding(Test&& arg) {
    std::cout << "Rvalue reference binding\n";
}

int main() {
    auto test = Test();
    auto test1 = test;
    auto test2 = Move(test);
    auto test3 = Move(std::move(test2));

    {
        auto test1 = Test();
        SomeFunc(test1);
        SomeFunc(Move(test1));
    }

    {
        auto test1 = Test();
        TestBinding(test1);
        TestBinding(Move(test1));

        auto test2 = Test();
        auto& test2_lvalue_ref = test2;
        TestBinding(test2_lvalue_ref);

        auto test3 = Test();
        Test&& test3_rvalue_ref = Move(test3);
        TestBinding(test3_rvalue_ref);
    }

    return 0;
}
