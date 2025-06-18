#include <iostream>

char const* GetStr() { return "some other string"; }

void Test0() {
    auto s = []() -> std::string_view { return "some string"; };

    std::cout << s() << std::endl;
    auto cstr = GetStr();
    std::cout << cstr << std::endl;
}

char const c_str[] = "some string";

struct LiteralString {
    template <typename N>
    consteval LiteralString(char const c_str[N])
};

int main() {
    Test0();
    Test1();

    return 0;
}
