#include "gtest/gtest.h"
#include "minaio/v2/context.h"

namespace minaio::v2 {

namespace {

void StartOneIteration(Context& ctx, int& counter) noexcept {
    if (counter == 10) {
        ctx.Stop();
        return;
    }

    counter++;

    ctx.Enqueue([&ctx, &counter] { StartOneIteration(ctx, counter); });
}

}  // namespace

TEST(MinaioSuite, ContextTestDummy) {
    Context ctx;

    int counter = 0;
    StartOneIteration(ctx, counter);

    while (!ctx.IsStopped()) {
        ctx.Poll();
    }

    ASSERT_EQ(counter, 10);
}

TEST(MinaioSuite, ContextOnly) {}

TEST(MinaioSuite, VerifyEnqueueSemantics) {}

}  // namespace minaio::v2

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
