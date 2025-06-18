#include "gtest/gtest.h"
#include "minaio/v2/datagram_socket.h"

using namespace minaio::v2;

namespace {

void Shutdown(Context& ctx) {
    static std::size_t counter = 0;
    counter++;

    if (counter == 2) {
        ctx.Stop();
    }
}

struct Ponger {
    Ponger(DatagramSocket&& socket, std::size_t nrounds)
        : socket_{std::move(socket)},
          nrounds_{nrounds},
          buffer_(1024, 0),
          remote_endpoint_{"", 0} {}

    void Start() { StartRead(); }

    void StartRead() {
        socket_.AsyncReadFrom(buffer_.data(), buffer_.size(), remote_endpoint_,
                              [this](core::ErrorOr<std::size_t> re) {
                                  ASSERT_EQ(re.Ok(), true);
                                  ASSERT_EQ(re.ValueUnsafe(), buffer_.size());
                                  StartWrite();
                              });
    }

    void StartWrite() {
        socket_.AsyncWriteTo(buffer_.data(), buffer_.size(), remote_endpoint_,
                             [this](core::ErrorOr<std::size_t> re) {
                                 ASSERT_EQ(re.Ok(), true);
                                 ASSERT_EQ(re.ValueUnsafe(), buffer_.size());

                                 iround_++;

                                 if (iround_ == nrounds_) {
                                     socket_.Close();
                                     Shutdown(socket_.context());
                                     return;
                                 }

                                 StartRead();
                             });
    }

    DatagramSocket socket_;
    std::size_t nrounds_;
    std::size_t iround_{0};
    std::vector<char> buffer_;
    Url remote_endpoint_;
};

struct Pinger {
    Pinger(DatagramSocket&& socket, Url const& u, std::size_t nrounds)
        : socket_{std::move(socket)},
          remote_endpoint_{u},
          nrounds_{nrounds},
          buffer_(1024, 0) {}

    void Start() { StartWrite(); }

    void StartRead() {
        socket_.AsyncReadFrom(buffer_.data(), buffer_.size(), &remote_endpoint_,
                              [this](core::ErrorOr<std::size_t> re) {
                                  ASSERT_EQ(re.Ok(), true);
                                  ASSERT_EQ(re.ValueUnsafe(), buffer_.size());

                                  iround_++;

                                  if (iround_ == nrounds_) {
                                      socket_.Close();
                                      Shutdown(socket_.context());
                                      return;
                                  }

                                  StartWrite();
                              });
    }

    void StartWrite() {
        socket_.AsyncWriteTo(buffer_.data(), buffer_.size(), remote_endpoint_,
                             [this](core::ErrorOr<std::size_t> re) {
                                 ASSERT_EQ(re.Ok(), true);
                                 ASSERT_EQ(re.ValueUnsafe(), buffer_.size());
                                 StartRead();
                             });
    }

    DatagramSocket socket_;
    Url remote_endpoint_;
    std::size_t nrounds_;
    std::size_t iround_{0};
    std::vector<char> buffer_;
};

}  // namespace

TEST(MinaioSuite, DatagramSocketPingPong) {
    Context ctx;

    auto ping_socket = DatagramSocket{ctx, Url{"0.0.0.0", 12345}};

    auto pong_socket = DatagramSocket{ctx, Url{"0.0.0.0", 12346}};

    constexpr std::size_t nrounds = 1000;

    Ponger ponger{std::move(ponger_socket), nrounds};
    ponger.Start();

    Pinger pinger{std::move(pinger_socket), ponger_url, nrounds};
    pinger.Start();

    while (!ctx.IsStopped()) {
        ctx.RunOne();
    }
    ASSERT_EQ(ctx.IsStopped(), true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
