#include <iostream>

int main(int argc, char* argv[]) {
    int k = 0x7fffffff;
    k += argc;

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
