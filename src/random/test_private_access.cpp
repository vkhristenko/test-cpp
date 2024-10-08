#include <iostream>

class Something {
public:
    Something() : value_{10} {}

private:
    int value_;
    friend class SomethingTesting;
};

struct SomethingTesting {
    Something& s;

    int& value() { return s.value_; }
};

void Test0() {
    Something dut;
    SomethingTesting testing{dut};
    std::cout << testing.value() << std::endl;
}

int main() {
    Test0();
    return 0;
}
