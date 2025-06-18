#pragma once

#include <string>
#include <string_view>

template <typename T>
struct ClassTemplateWithStaticDataMember {
    inline static std::string sMem = "something";
    constexpr static std::string_view sStringView = "something else";
    static std::string sMem1;
    static const int non_addressable = 10;
};

static void SomeFunction() {}

#define PRINTME(VAR) std::cout << #VAR " = " << VAR << std::endl
