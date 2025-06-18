#include <queue>

#include "gtest/gtest.h"

template <typename T>
class Queue {
public:
    Queue() {}

    void Enqueue(T const& e) { q_.push(e); }
    T Dequeue() {
        auto e = std::move(q_.front());
        q_.pop();
        return std::move(e);
    }
    std::size_t size() const { return q_.size(); }

private:
    std::queue<T> q_;
};

class QueueTest : public testing::Test {
public:
    void SetUp() override {
        for (std::size_t i = 0; i < 10; i++) {
            q_.Enqueue(i);
        }
    }

    void TearDown() override {}

    Queue<std::size_t> q_;
};

TEST_F(QueueTest, IsEmpty) { EXPECT_EQ(q_.size(), 10); }

TEST_F(QueueTest, DequeueWorks) {
    EXPECT_EQ(q_.size(), 10);

    std::size_t i = 9;
    while (auto value = q_.Dequeue()) {
        EXPECT_EQ(value, i--);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
