#include <iostream>

int main() {
    int* a = new int[10];
    a[5] = 10;
    if (a[0]) {
        std::cout << "a[0] is non-zero" << std::endl;
    } else {
        std::cout << "a[0] is zero" << std::endl;
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
