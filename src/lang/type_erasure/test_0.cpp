#include <iostream>

struct Base;

using FuncType = void(Base*);

struct Base {
    void run() { func_(this); }

protected:
    Base(FuncType* func) : func_(func) {}

    FuncType* func_;
};

struct Derived : Base {
    Derived() : Base(&Derived::do_run) {}

    static void do_run(Base* self) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

void Test0() {
    auto derived = Derived{};
    Base& base = derived;
    base.run();
}

struct QueryBase {
    virtual void const* ClassId() const noexcept { return &s_id; }

    virtual bool isA(void const* class_id) const noexcept {
        return class_id == ClassId();
    }

    template <typename T>
    bool isA() const noexcept {
        return isA(T::ClassId());
    }

    template <typename T>
    T const* as() const noexcept {
        if (!isA<T>()) {
            return nullptr;
        }

        return static_cast<T const*>(this);
    }

    inline static char s_id = 0;
    inline static void const* const kType = &s_id;
};

struct ByExchange : QueryBase {
    ByExchange(size_t i) : i_{i} {}

    inline static char s_id = 0;
    inline static void const* kType = &s_id;

    size_t i_;
};

struct BySecId : QueryBase {
    inline static char s_id = 0;
    inline static void const* kType = &s_id;
};

void Test1() {
    auto type = ByExchange{10};
    [](QueryBase const& query) {
        if (query.isA<ByExchange>()) {
            std::cout << "by exchange" << std::endl;
        } else if (query.isA<BySecId>()) {
            std::cout << "by sec id " << std::endl;
        } else {
            std::cout << "not supported" << std::endl;
        }
    }(type);
}

int main() {
    Test0();
    Test1();
}
