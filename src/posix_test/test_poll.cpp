#include <iostream>
#include <vector>

// TODO remove what's not needed
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>

#include "fmt/core.h"

int RunServer(unsigned int port) {
    auto fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        ::fmt::print("socket error! error = {}\n", strerror(errno));
        return 1;
    }   

    //
    // Make this file descriptor non blocking
    // TODO is this necessary?
    //
    auto ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    if (ret == -1) {
        ::fmt::print("fcntl error! error = {}\n", strerror(errno));
        return 1;
    }

    //
    // Prepare this socket for accepting connections
    //
    sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr = in_addr{0};
    if (bind(fd, reinterpret_cast<sockaddr const*>(&sin), sizeof(sin)) < 0) {
        ::fmt::print("bind error: {}\n", strerror(errno));
        return 1;
    }   
    if (listen(fd, 10) < 0) {
        ::fmt::print("listen error {}", strerror(errno));
        return 1;
    }

    std::vector<pollfd> all_pfds;
    pollfd server_pfd;
    server_pfd.fd = fd;
    server_pfd.events = POLLIN;
    all_pfds.push_back(std::move(server_pfd));

    while (1) {
        // timeout of 0 => poll returns immediately even if no fds are ready
        auto ready = poll(all_pfds.data(), all_pfds.size(), 0);
        
        if (ready == -1) {
            ::fmt::print("poll error: {}\n", strerror(errno));
            return 1;
        }

        if (ready == 0) {
            ::fmt::print("no events!\n");
            continue;
        }

        if (all_pfds[0].revents & POLLIN) {
            auto sfd = accept(all_pfds[0].fd, nullptr, nullptr);
            if (sfd == -1) {
                ::fmt::print("accept failed! chcking if this is just no conn!\n");
                if (errno != EAGAIN && errno != EWOULDBLOCK){
                    ::fmt::print("accept failed {}.\n", strerror(errno));
                    return 1;
                }

                continue;
            }

            ::fmt::print("valid conenction from {}\n", sfd);
            return 0;
        }
    }
}

int main() {
    return RunServer(12345);
}
