#include <iostream>

#include "lang_test/linkage/static_members/src1.h"
#include "lang_test/linkage/static_members/src2.h"

void Test0() {
    std::cout << &one::getStaticMember() << std::endl;
    std::cout << &two::getStaticMember() << std::endl;
}

void Test1() {
    std::cout << &one::getStaticMemberStringView() << std::endl;
    std::cout << &two::getStaticMemberStringView() << std::endl;
}

int main() {
    Test0();
    Test1();
    return 0;
}
