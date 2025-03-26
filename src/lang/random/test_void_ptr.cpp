#include <iostream>

void Test0() {
    int a = 10;
    void* ptr = reinterpret_cast<void*>(a);
    std::cout << ptr << std::endl;
    std::cout << reinterpret_cast<int>(ptr);
}

int main() {
    Test0();

    return 0;
}
