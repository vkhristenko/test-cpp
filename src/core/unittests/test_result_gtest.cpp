#include <gtest/gtest.h>
#include "core/result.h"
#include "core/unique_ref.h"

#include <string>
#include <memory>

using core::Result;

TEST(ResultTest, ValueAndError) {
    Result<int, std::string> ok(123);
    Result<int, std::string> err(std::string("fail"));
    EXPECT_TRUE(ok.HasValue());
    EXPECT_FALSE(ok.HasError());
    EXPECT_EQ(ok.Value(), 123);
    EXPECT_TRUE(err.HasError());
    EXPECT_FALSE(err.HasValue());
    EXPECT_EQ(err.Error(), "fail");
}

TEST(ResultTest, MoveSemantics) {
    Result<std::string, int> ok(std::string("abc"));
    Result<std::string, int> moved = std::move(ok);
    EXPECT_TRUE(moved.HasValue());
    EXPECT_EQ(moved.Value(), "abc");
}

TEST(ResultTest, ThrowsOnWrongAccess) {
    Result<int, std::string> ok(1);
    Result<int, std::string> err(std::string("bad"));
    EXPECT_THROW(err.Value(), std::logic_error);
    EXPECT_THROW(ok.Error(), std::logic_error);
}

TEST(ResultTest, VoidErrorSpecialization) {
    Result<int, void> ok(42);
    EXPECT_TRUE(ok.HasValue());
    EXPECT_EQ(ok.Value(), 42);
}

TEST(ResultTest, VoidValueSpecialization) {
    Result<void, std::string> err(std::string("fail"));
    EXPECT_TRUE(err.HasError());
    EXPECT_EQ(err.Error(), "fail");
}

TEST(ResultTest, RvalueAccess) {
    Result<std::string, int> ok(std::string("move"));
    std::string s = std::move(ok).Value();
    EXPECT_EQ(s, "move");
}

TEST(ResultTest, ConvertingConstructorValue) {
    // int -> double
    Result<double, std::string> r1(42); // int convertible to double
    EXPECT_TRUE(r1.HasValue());
    EXPECT_EQ(r1.Value(), 42.0);

    // const char* -> std::string
    Result<int, std::string> r2("error");
    EXPECT_TRUE(r2.HasError());
    EXPECT_EQ(r2.Error(), "error");
}

TEST(ResultTest, ConvertingConstructorError) {
    // int -> double error
    Result<std::string, double> r1(3.14); // double convertible to error
    EXPECT_TRUE(r1.HasError());
    EXPECT_EQ(r1.Error(), 3.14);

    // const char* -> std::string error
    Result<int, std::string> r2(std::string("fail"));
    Result<int, std::string> r3 = std::string("fail2");
    EXPECT_TRUE(r2.HasError());
    EXPECT_EQ(r2.Error(), "fail");
    EXPECT_TRUE(r3.HasError());
    EXPECT_EQ(r3.Error(), "fail2");
}

struct AbstractBase {
    virtual int get() const = 0;
    virtual ~AbstractBase() = default;
};

struct Concrete : AbstractBase {
    int value;
    explicit Concrete(int v) : value(v) {}
    int get() const override { return value; }
};

TEST(ResultTest, UniquePtrToResultAbstractBase) {
    std::unique_ptr<AbstractBase> uptr(new Concrete(123));
    Result<std::unique_ptr<AbstractBase>, std::string> r(std::move(uptr));
    ASSERT_TRUE(r.HasValue());
    EXPECT_EQ(r.Value()->get(), 123);
}

TEST(ResultTest, UniquePtrToResultDerived) {
    std::unique_ptr<Concrete> uptr(new Concrete(456));
    Result<std::unique_ptr<Concrete>, std::string> r(std::move(uptr));
    ASSERT_TRUE(r.HasValue());
    EXPECT_EQ(r.Value()->get(), 456);
}

TEST(ResultTest, ResultWithAbstractBaseError) {
    Result<int, std::unique_ptr<AbstractBase>> r(std::unique_ptr<AbstractBase>(new Concrete(789)));
    ASSERT_TRUE(r.HasError());
    EXPECT_EQ(r.Error()->get(), 789);
}

TEST(ResultTest, ConstructWithUniquePtrDerivedToBaseValue) {
    std::unique_ptr<Concrete> derived_ptr(new Concrete(111));
    Result<std::unique_ptr<AbstractBase>, std::string> r(std::move(derived_ptr));
    ASSERT_TRUE(r.HasValue());
    EXPECT_EQ(r.Value()->get(), 111);
}

TEST(ResultTest, ConstructWithUniquePtrDerivedToBaseError) {
    std::unique_ptr<Concrete> derived_ptr(new Concrete(222));
    Result<int, std::unique_ptr<AbstractBase>> r(std::move(derived_ptr));
    ASSERT_TRUE(r.HasError());
    EXPECT_EQ(r.Error()->get(), 222);
}

struct RefBase {
    virtual int get() const = 0;
    virtual ~RefBase() = default;
};

struct RefDerived : RefBase {
    int value;
    explicit RefDerived(int v) : value(v) {}
    int get() const override { return value; }
};

TEST(ResultTest, UniqueRefBaseFromDerivedValue) {
    core::UniqueRef<RefDerived> ref_d(*new RefDerived(321));
    Result<core::UniqueRef<RefBase>, std::string> r(std::move(ref_d));
    ASSERT_TRUE(r.HasValue());
    EXPECT_EQ(r.Value().get().get(), 321);
}

TEST(ResultTest, UniqueRefBaseFromDerivedError) {
    core::UniqueRef<RefDerived> ref_d(*new RefDerived(654));
    Result<int, core::UniqueRef<RefBase>> r(std::move(ref_d));
    ASSERT_TRUE(r.HasError());
    EXPECT_EQ(r.Error().get().get(), 654);
}

static_assert(std::is_convertible_v<core::UniqueRef<RefDerived>, core::UniqueRef<RefBase>>,
              "UniqueRef<RefDerived> should be convertible to UniqueRef<RefBase>");
