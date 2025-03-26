#include <iostream>

struct Base {
private:
    int a;

    friend struct Access;
};

struct Derived : protected Base {
    friend struct Access;
};

struct Access {
    static int GetA(Base& base) {
        return base.a;
    }

    static int GetA(Derived& d) {
        return d.a;
    }
};

void Test0() {
    auto derived = Derived{};
    std::cout << Access::GetA(derived) << std::endl;
}

int main() {
    Test0();

    return 0;
}
