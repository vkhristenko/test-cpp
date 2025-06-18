#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "fmt/core.h"

void RunServer(unsigned int port) {
    auto fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        ::fmt::print("socket error! error = {}\n", strerror(errno));
        return;
    }

    auto ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    if (ret == -1) {
        ::fmt::print("fcntl error! error = {}\n", strerror(errno));
        return;
    }

    sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr = in_addr{0};
    if (bind(fd, reinterpret_cast<sockaddr const*>(&sin), sizeof(sin)) < 0) {
        ::fmt::print("bind error: {}\n", strerror(errno));
        return;
    }

    if (listen(fd, 10) < 0) {
        ::fmt::print("listen error {}", strerror(errno));
        return;
    }

    while (1) {
        auto sfd = accept(fd, nullptr, nullptr);
        if (sfd == -1) {
            ::fmt::print("accept failed! chcking if this is just no conn!\n");
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                ::fmt::print("accept failed {}.\n", strerror(errno));
                return;
            }

            continue;
        }

        ::fmt::print("valid conenction from {}\n", sfd);
        return;
    }
}

int main() {
    RunServer(12345);
    return 0;
}
