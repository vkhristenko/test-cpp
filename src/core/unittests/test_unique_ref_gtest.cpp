#include "core/unique_ref.h"
#include <gtest/gtest.h>
#include <string>
#include <memory>

struct Dummy {
    int x;
    Dummy(int v) : x(v) {}
    Dummy(Dummy&& o) noexcept : x(o.x) { o.x = -1; }
    Dummy& operator=(Dummy&& o) noexcept { x = o.x; o.x = -1; return *this; }
    Dummy(const Dummy&) = delete;
    Dummy& operator=(const Dummy&) = delete;
};

struct Base {
    virtual ~Base() = default;
    int y = 0;
};

struct Derived : Base {
    int z = 0;
    Derived(int y_, int z_) { y = y_; z = z_; }
};

struct AbstractBase {
    virtual int get() const = 0;
    virtual ~AbstractBase() = default;
};

struct Concrete : AbstractBase {
    int value;
    explicit Concrete(int v) : value(v) {}
    int get() const override { return value; }
};

TEST(UniqueRefTest, BasicMoveAndAccess) {
    core::UniqueRef<Dummy> ref1(*new Dummy(42));
    EXPECT_EQ(ref1->x, 42);
    EXPECT_EQ((*ref1).x, 42);
    ref1.get().x = 100;
    EXPECT_EQ(ref1->x, 100);
}

TEST(UniqueRefTest, MoveSemantics) {
    core::UniqueRef<Dummy> ref1(*new Dummy(7));
    core::UniqueRef<Dummy> ref2 = std::move(ref1);
    EXPECT_EQ(ref2->x, 7);
}

TEST(UniqueRefTest, MoveAssignment) {
    core::UniqueRef<Dummy> ref1(*new Dummy(1));
    core::UniqueRef<Dummy> ref2(*new Dummy(2));
    ref2 = std::move(ref1);
    EXPECT_EQ(ref2->x, 1);
}

TEST(UniqueRefTest, Release) {
    core::UniqueRef<Dummy> ref1(*new Dummy(55));
    auto& raw = ref1.release();
    EXPECT_EQ(raw.x, 55);
    delete &raw;
}

TEST(UniqueRefTest, ConvertingConstructorUpcast) {
    Derived* d = new Derived(5, 10);
    core::UniqueRef<Base> ref(*d); // direct from Derived to Base
    EXPECT_EQ(ref->y, 5);
    // Should be able to access Base part, not Derived
}

TEST(UniqueRefTest, ConvertingConstructorTemplate) {
    Derived* d = new Derived(7, 20);
    core::UniqueRef<Derived> ref_d(*d);
    core::UniqueRef<Base> ref_b(ref_d.release()); // Use converting constructor
    EXPECT_EQ(ref_b->y, 7);
}

TEST(UniqueRefTest, UniquePtrToUniqueRefAbstractBase) {
    std::unique_ptr<AbstractBase> uptr(new Concrete(123));
    // Transfer ownership from unique_ptr to UniqueRef<AbstractBase>
    core::UniqueRef<AbstractBase> ref(*uptr.release());
    EXPECT_EQ(ref->get(), 123);
}

TEST(UniqueRefTest, UniquePtrToUniqueRefDerived) {
    std::unique_ptr<Concrete> uptr(new Concrete(456));
    core::UniqueRef<Concrete> ref(*uptr.release());
    EXPECT_EQ(ref->get(), 456);
}
