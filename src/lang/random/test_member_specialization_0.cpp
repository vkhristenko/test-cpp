#include <iostream>
#include <print>

#include "core/macros.h"

template<typename T>
struct Factory {
    template<typename... Args>
    static T make(Args&&... args) {
        TCPP_PRINT_HERE();
        return T(std::forward<Args>(args)...);
    }
};

template<>
template<>
std::string Factory<std::string>::make(std::string const& s) {
    TCPP_PRINT_HERE();
    return s;
}

template<>
template<>
std::string Factory<std::string>::make(std::string&& s) {
    TCPP_PRINT_HERE();
    return s;
}

void Test0() {
    auto str = Factory<std::string>::make("something");
    std::println("str={}", str);

    auto str1 = Factory<std::string>::make(std::string{"something else"});
    std::println("str={}", str1);
}

template<typename T>
void Test1Impl(T) {
    TCPP_PRINT_HERE();
}

template<>
void Test1Impl<int>(int a) {
    TCPP_PRINT_HERE();
}

void Test1() {
    Test1Impl(1);
    Test1Impl("something");
    Test1Impl(std::string("here"));
}

int main() {
    Test0();
    Test1();

    return 0;
}
