#include <iostream>
#include <vector>

class std_interface {
public:
    virtual void start() = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void quit() = 0;
    virtual void full_size() = 0;
    virtual void small() = 0;

    virtual ~std_interface() {}
};

class some_interface : public std_interface {
public:
    void start() {}
    void suspend() {}
    void resume() {}
    void quit() {}
    void full_size() {}
    void small() {}

    ~some_interface() {}
};

void test(std_interface *interface) {
    using p_std_int = void (std_interface::*)();

    p_std_int s = &std_interface::suspend;
    interface->suspend();
    (interface->*s)();
}

void test(std_interface &interface) {
    using p_std_int = void (std_interface::*)();

    p_std_int s = &std_interface::suspend;
    interface.suspend();
    (interface.*s)();
}

class text : public std_interface {
public:
    void start() override {}
    void suspend() override {}
    virtual void print() {}

private:
    std::vector<int> s;
};

int main() {
    // contravariance
    using p_std_int = void (std_interface::*)();
    // cannot initialize a variable of type
    //       'void (std_interface::*)()' with an rvalue of type 'void (text::*)()': different classes
    //             ('std_interface' vs 'text')
//    void (std_interface::*p1)() = &text::print;

    void (text::*p2)() = &std_interface::start;
    return 0;
}
