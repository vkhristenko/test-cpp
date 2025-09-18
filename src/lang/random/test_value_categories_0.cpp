#include <iostream>

template<typename T>
struct is_lvalue : std::false_type {};

template<typename T>
struct is_lvalue<T&> : std::true_type {};

template<typename T>
struct is_xvalue : std::false_type {};

template<typename T>
struct is_xvalue<T&&> : std::true_type {};

template<typename T>
struct is_prvalue : std::true_type {};

template<typename T>
struct is_prvalue<T&> : std::false_type {};

template<typename T>
struct is_prvalue<T&&> : std::false_type {};

/// 1. Value categories are for expressions.
/// 2. References are related but different concept.
///
/// There are 3 value cateogires: 
/// lvalue, xvalue, and prvalue
///
/// The way you check what is what involves references
/// When passing a variable into a function, we really pass an expression,
/// at this point we need to think about expressions, expression's type and value category of expressions rather than variables and types of variables.

/// decltype(auto) - decltype of the expression, preserves the value category of the expression
///
/// auto&& - is a universal reference and performs deducation

void Test0() {
    static_assert(is_lvalue<int&>::value, "int& is lvalue");
    static_assert(!is_lvalue<int&&>::value, "int&& is not lvalue");
    static_assert(!is_lvalue<int>::value, "int is not lvalue");

    int a = 0;
    int& b = a;
    int&& c = std::move(a);

    static_assert(is_lvalue<decltype((a))>::value, "(a) is lvalue");
    static_assert(is_lvalue<decltype((b))>::value, "(b) is lvalue");
    static_assert(is_lvalue<decltype((c))>::value, "(c) is lvalue");

    static_assert(is_xvalue<decltype(std::move(a))>::value, "std::move(a) is xvalue");
    static_assert(is_xvalue<decltype((static_cast<int&&>(a)))>::value, "static_cast<int&&>(a) is xvalue");

    static_assert(is_prvalue<decltype((42))>::value, "42 is prvalue");
    static_assert(! is_prvalue<decltype((a))>::value, "(a) is not prvalue");
    static_assert(is_prvalue<decltype((a + 1))>::value, "(a + 1) is prvalue");
    static_assert(is_prvalue<decltype((a++))>::value, "(a++) is prvalue");

    struct Test {
        int x;
    };

    static_assert(is_prvalue<decltype(Test{42})>::value, "Test{42} is prvalue");
    static_assert(! is_prvalue<decltype((Test{42}.x))>::value, "Test{42}.x is not prvalue");
    static_assert(is_xvalue<decltype((Test{42}.x))>::value, "static_cast<int&&>(Test{42}.x) is xvalue");

    auto test = Test{42};
    auto&& test_rvalue_ref = std::move(test);
    static_assert(is_lvalue<decltype((test_rvalue_ref))>::value, "(test_rvalue_ref) is xvalue");
    static_assert(is_xvalue<decltype(std::move(test_rvalue_ref))>::value, "std::move(test_rvalue_ref) is xvalue");

    decltype(auto) i = (a);
    decltype(auto) j = std::move(a);
    static_assert(is_lvalue<decltype((i))>::value, "(i) is lvalue");
    static_assert(is_xvalue<decltype(std::move(j))>::value, "(j) is xvalue");
}

int main() {
    Test0();

    return 0;
}
