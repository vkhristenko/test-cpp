#include <iostream>

#define APPLY_ending()

#define TEST_EXISTS(ENDING) APPLY_##ENDING()

void Test0() { TEST_EXISTS(ending); }

int main() {
    Test0();

    return 0;
}
