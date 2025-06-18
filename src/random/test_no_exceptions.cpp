#include <iostream>
#include <stdexcept>

void ThrowSomething() { throw std::runtime_error{"some error"}; }

int main() { return 0; }
