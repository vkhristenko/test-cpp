#include <iostream>

// check fibonacci values here for testing
// https://en.wikipedia.org/wiki/Fibonacci_sequence
int main() {
    int A;
    std::cin >> A;

    int f_n_2 = 0;
    int f_n_1 = 1;
    int n = 2;
    int f_n = 1;
    while (f_n < A) {
        f_n_2 = f_n_1;
        f_n_1 = f_n;
        f_n = f_n_1 + f_n_2;
        n++;
    }

    if (f_n == A) {
        std::cout << n << std::endl;
    } else {
        std::cout << -1 << std::endl;
    }
}
