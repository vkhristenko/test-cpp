#pragma once

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <functional>
#include <queue>
#include <string>
#include <unordered_map>

#include "core/macros.h"
#include "fmt/core.h"
#include "minaio/endpoint.h"

namespace minaio {

class IOContext;

class EventHandler {
public:
    virtual ~EventHandler() noexcept = default;

    virtual void Handle(epoll_event const&) noexcept = 0;

protected:
    core::Result<int, std::string> Register(IOContext& io_ctx, int fd,
                                            epoll_event& e);
    core::Result<int, std::string> Deregister(IOContext& io_ctx, int fd);
    core::Result<int, std::string> Modify(IOContext& io_ctx, int fd,
                                          epoll_event&);
    void EnqueueCompletionHandler(IOContext& io_ctx, std::function<void()> cb);
};

class IOContext final {
public:
    IOContext() {
        TCPP_PRINT_PRETTY_FUNCTION();

        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ == -1) {
            ::fmt::print("epoll_create error! error = {}\n", strerror(errno));
            throw std::runtime_error{"epoll_create error"};
        }
    }

    ~IOContext() noexcept { TCPP_PRINT_PRETTY_FUNCTION(); }

    void Poll() {
        // TCPP_PRINT_PRETTY_FUNCTION();

        //
        //
        //
        while (!completion_handlers_.empty()) {
            auto op = std::move(completion_handlers_.front());
            op();
            completion_handlers_.pop();
        }

        while (MoreEvents()) {
            for (auto const& e : epoll_events_) {
                auto it = io_handlers_.find(e.data.fd);

                if (it == io_handlers_.end()) {
                    continue;
                }

                it->second->Handle(e);
            }
        }
    }

    bool MoreToComplete() const noexcept {
        return completion_handlers_.size() > 0;
    }

private:
    bool MoreEvents() {
        // TCPP_PRINT_PRETTY_FUNCTION();

        auto nfds = epoll_wait(epoll_fd_, epoll_events_.data(),
                               epoll_events_.size(), 0);

        if (nfds <= -1) {
            ::fmt::print("nfds ({}) <= -1", nfds);
            throw std::runtime_error{"failed epoll_wait"};
        }

        if (nfds == 0) {
            return false;
        }

        return true;
    }

private:
    // TODO should be void
    core::Result<int, std::string> RegisterEventHandler(
        int fd, epoll_event& event, EventHandler* handler) noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        auto res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
        if (res == -1) {
            ::fmt::print("epoll_ctl error {}\n", strerror(errno));
            return std::string{"epoll_ctl error"};
        }

        auto it = io_handlers_.find(fd);
        if (it != io_handlers_.end()) {
            return std::string{"io handler already exists"};
        }

        io_handlers_[fd] = handler;
        return 0;
    }

    // TODO should be void
    core::Result<int, std::string> DeregisterEventHandler(int fd) noexcept {
        TCPP_PRINT_PRETTY_FUNCTION();
        epoll_event e;
        auto res = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &e);
        if (res <= -1) {
            ::fmt::print("epoll_ctl error {}", strerror(errno));
            return std::string{"epoll_ctl error"};
        }

        auto it = io_handlers_.find(fd);
        if (it == io_handlers_.end()) {
            return std::string{"no such io handler"};
        }

        io_handlers_.erase(fd);
        return 0;
    }

private:
    //
    // epoll instance
    //
    int epoll_fd_;

    constexpr static auto kMaxEventsPerPoll = 100u;
    std::array<epoll_event, kMaxEventsPerPoll> epoll_events_;
    std::unordered_map<int, EventHandler*> io_handlers_;

    //
    //
    //
    std::queue<std::function<void()>> completion_handlers_;

    friend class EventHandler;
};

core::Result<int, std::string> EventHandler::Register(IOContext& io_ctx, int fd,
                                                      epoll_event& e) {
    TCPP_PRINT_PRETTY_FUNCTION();
    return io_ctx.RegisterEventHandler(fd, e, this);
}

core::Result<int, std::string> EventHandler::Deregister(IOContext& io_ctx,
                                                        int fd) {
    TCPP_PRINT_PRETTY_FUNCTION();
    return io_ctx.DeregisterEventHandler(fd);
}

void EventHandler::EnqueueCompletionHandler(IOContext& io_ctx,
                                            std::function<void()> cb) {
    io_ctx.completion_handlers_.push(std::move(cb));
}

}  // namespace minaio
