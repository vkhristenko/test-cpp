#include "minaio/v2/reactor.h"

#include "core/macros.h"

namespace minaio::v2 {

void Reactor::Add(
        int fd, 
        std::function<void(std::uint8_t)> fd_handler,
        std::uint8_t fd_flag) noexcept {
    /// TODO propogate failure to the user in case flags do not make snse
    pollfd pfd;
    pfd.fd = fd;
    pfd.events = 0;
    pfd.events |= fd_flag;
    pfd.revents = 0;

    fd_data_.push_back(pfd);
    fd_handlers_[fd] = std::move(fd_handler);
}

void Reactor::Remove(int fd) noexcept {
    std::erase_if(fd_data_, [fd](auto const& pfd) { return fd == pfd.fd; });
    fd_handlers_.erase(fd);
}

void Reactor::Poll() noexcept {
    auto const re = poll(fd_data_.data(), fd_data_.size(), 0);
    TCPP_ABORT_IF(re < 0, "poll failed!");
    if (re == 0) {
        return ;
    }

    for (std::size_t i=0; i<fd_data_.size();) {
        std::uint8_t event = 0u;

        if (fd_data_[i].revents & POLLIN) {
            event |= static_cast<std::uint8_t>(ReactorEvent::kRead);
        }

        if (fd_data_[i].revents & POLLOUT) {
            event |= static_cast<std::uint8_t>(ReactorEvent::kWrite);
        }

        if (fd_data_[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
            event |= static_cast<std::uint8_t>(ReactorEvent::kClose);
        }

        /// TODO 
        /// it's probably better to have access to an event loop and to enqueue
        /// callbacks. Otherwise we need to think about a possibility a user removing
        /// an fd during the iteration.
        auto& handler = fd_handlers_[fd_data_[i].fd];
        auto const size = fd_data_.size();
        handler(event);

        if (size == fd_data_.size()) {
            i++;
        }
    }
}

}
