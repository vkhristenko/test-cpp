#include <iostream>
#include <array>
#include <vector>

// TODO remove what's not needed
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/epoll.h>

#include "fmt/core.h"

int RunServer(unsigned int port) {
    //
    // new file descriptor for stream socket
    //
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

    //
    // create epoll instance
    //
    auto epollfd = epoll_create1(0);
    if (epollfd == -1) {
        ::fmt::print("socket error! error = {}\n", strerror(errno));
        return 1;
    }

    // 
    // add a socket to the interest list
    //
    epoll_event eevent;
    eevent.events = EPOLLIN;
    eevent.data.fd = fd;
    auto res = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &eevent);
    if (res == -1) {
        ::fmt::print("epoll_ctl error {}", strerror(errno));
        return 1;
    }

    std::array<epoll_event, 100> events; 
    for (;;) {
        auto nfds = epoll_wait(epollfd, events.data(), events.size(), 0);

        if (nfds <= -1) {
            ::fmt::print("epoll_wait error: {}\n", strerror(errno));
            return 1;
        }

        if (nfds == 0) {
            ::fmt::print("no events");
            continue;
        }

        for (auto& e : events) {
            if (e.data.fd == fd) {
                ::fmt::print("new event for fd = {}\n", fd); 

                auto client_sock = accept(fd, nullptr, nullptr);
                if (client_sock == -1) {
                    ::fmt::print("accept failed! chcking if this is just no conn!\n");
                    if (errno != EAGAIN && errno != EWOULDBLOCK){
                        ::fmt::print("accept failed {}.\n", strerror(errno));
                        return 1;
                    }  

                    continue;
                }

                ::fmt::print("valid connection from {}\n", client_sock);
                return 0;
            } else {
                ::fmt::print("event for unknown file descriptor {}\n", 
                    static_cast<int>(e.data.fd));
                return 1;
            }
        }
    }

    return 0;
}

int main() {
    return RunServer(12345);
}
