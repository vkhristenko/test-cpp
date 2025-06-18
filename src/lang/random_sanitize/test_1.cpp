#include <iostream>

int* GetIntPtr() {
    int value = 10;
    return &value;
}

int main() {
    std::cout << *GetIntPtr() << std::endl;

    return 0;
}
