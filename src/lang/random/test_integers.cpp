#include <iostream>
#include <limits>

void Test0() {
    std::cout << std::numeric_limits<int64_t>::min(); 
}

int main() {
    Test0();

    return 0;
}
