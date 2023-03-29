#include <iostream>

#include "gtest/gtest.h"
#include "protos/dummy/dummy_1.pb.h"

using namespace protos::dummy;

TEST(Dummy1OneTest, CanSetAndGetFields) {
    Dummy1One dummy;
    dummy.set_f1("Hello");
    dummy.set_f2(123);
    dummy.set_f3(4.56);

    Dummy0One* something = dummy.mutable_something();
    something->set_f1("World");
    something->set_f2(789);
    something->set_f3(1.23);

    EXPECT_EQ(dummy.f1(), "Hello");
    EXPECT_EQ(dummy.f2(), 123);
    EXPECT_EQ(dummy.f3(), 4.56);
    EXPECT_EQ(dummy.something().f1(), "World");
    EXPECT_EQ(dummy.something().f2(), 789);
    EXPECT_EQ(dummy.something().f3(), 1.23);
}

TEST(Dummy1TwoTest, CanSetAndGetFields) {
    Dummy1Two dummy;
    Dummy1One* dummy_one = dummy.mutable_dummy_one();
    dummy_one->set_f1("Hello");
    dummy_one->set_f2(123);
    dummy_one->set_f3(4.56);

    Dummy0One* something = dummy_one->mutable_something();
    something->set_f1("World");
    something->set_f2(789);
    something->set_f3(1.23);

    dummy.add_repeated_ints(1);
    dummy.add_repeated_ints(2);
    dummy.add_repeated_ints(3);

    EXPECT_EQ(dummy.dummy_one().f1(), "Hello");
    EXPECT_EQ(dummy.dummy_one().f2(), 123);
    EXPECT_EQ(dummy.dummy_one().f3(), 4.56);
    EXPECT_EQ(dummy.dummy_one().something().f1(), "World");
    EXPECT_EQ(dummy.dummy_one().something().f2(), 789);
    EXPECT_EQ(dummy.dummy_one().something().f3(), 1.23);
    EXPECT_EQ(dummy.repeated_ints(0), 1);
    EXPECT_EQ(dummy.repeated_ints(1), 2);
    EXPECT_EQ(dummy.repeated_ints(2), 3);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
