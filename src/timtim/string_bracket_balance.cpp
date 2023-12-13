#include <iostream>

int main() {

    char str[] = "some string";
   
    // just a simple way to get a size of the above string
    // iterate until u find the zero-termination char
    int size = 0;
    while (str[size++] != '\0');
    // subtract so that we do not account for '\0'
    size--;

    int n = 0;
    for (int i=0; i<size; i++) {
        if (str[i] == '(') {
            n++;
        }

        if (str[i] == ')') {
            n--;
        }

        if (n < 0) {
            break;
        }
    }

    if (n != 0) {
        std::cout << "no" << std::endl;
    } else {
        std::cout << "yes" << std::endl;
    }

    return 0;
}
