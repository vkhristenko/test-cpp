#include <iostream>

#include "gtest/gtest.h"
#include "protos/dummy/dummy_0.pb.h"

using namespace protos::dummy;

TEST(Dummy0OneTest, CanSetAndGetFields) {
    Dummy0One dummy;
    dummy.set_f1("Hello");
    dummy.set_f2(123);
    dummy.set_f3(4.56);

    EXPECT_EQ(dummy.f1(), "Hello");
    EXPECT_EQ(dummy.f2(), 123);
    EXPECT_EQ(dummy.f3(), 4.56);
}

TEST(Dummy0TwoTest, CanSetAndGetFields) {
    Dummy0Two dummy;
    Dummy0One* dummy_one = dummy.mutable_dummy_one();
    dummy_one->set_f1("World");
    dummy_one->set_f2(789);
    dummy_one->set_f3(1.23);
    dummy.add_repeated_ints(1);
    dummy.add_repeated_ints(2);
    dummy.add_repeated_ints(3);

    EXPECT_EQ(dummy.dummy_one().f1(), "World");
    EXPECT_EQ(dummy.dummy_one().f2(), 789);
    EXPECT_EQ(dummy.dummy_one().f3(), 1.23);
    EXPECT_EQ(dummy.repeated_ints(0), 1);
    EXPECT_EQ(dummy.repeated_ints(1), 2);
    EXPECT_EQ(dummy.repeated_ints(2), 3);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
