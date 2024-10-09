#include <iostream>

#include "enum_gen_test/defs_generated.h"

int main() {
    std::cout << ToStringView(enum1::one) << std::endl;
    std::cout << ToStringView(static_cast<enum1>(10)) << std::endl;

    return 0;
}
