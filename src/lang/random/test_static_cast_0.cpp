#define FOR_RUNNING 1

#if defined(FOR_RUNNING)
#include <iostream>
#endif

struct Base1 {
    int xx, yy, zz;
};

struct Base2 {
    int aa, bb, cc;
};

struct Data : Base1, Base2 {
    double x,y,z;
    char data[10];
};

int Test0() {
    return sizeof(Data);
}

void Test1() {
#if defined(FOR_RUNNING)
    auto data = Data{};
    auto* base1 = static_cast<Base1*>(&data);
    auto* base2 = static_cast<Base2*>(&data);

    std::cout << &data << "  " << base1 << "  " << base2 << std::endl;
#endif
}

struct IBase1 {
    int aa, bb, cc;

    virtual ~IBase1() {}

    virtual void process1() {}
};

struct IBase2 {
    int xx, yy, zz;

    virtual ~IBase2() {}

    virtual void process2() {}
};

struct Derived : IBase1, IBase2 {
    double x, y, z;
};

int Test2() {
    return sizeof(Derived);
}

void Test3() {
#if defined(FOR_RUNNING)
    auto derived = Derived {};
    
    auto* base1 = static_cast<IBase1*>(&derived);
    auto* base2 = static_cast<IBase2*>(&derived);

    std::cout << &derived<< "  " << base1 << "  " << base2 << std::endl;
#endif
}

int main() {
#if defined(FOR_RUNNING)
    Test1();
    Test3();
#endif

    return Test0() && Test2();
}
