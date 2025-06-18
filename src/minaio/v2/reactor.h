#pragma once

#include <poll.h>

#include <unordered_map>
#include <vector>

namespace minaio::v2 {

using ReactorEvent = std::uint8_t;
constexpr std::uint8_t kNone = 0;
constexpr std::uint8_t kRead = 1 << 0;
constexpr std::uint8_t kWrite = 1 << 1;
constexpr std::uint8_t kClose = 1 << 2;

constexpr bool IsReadEvent(std::uint8_t mask) noexcept { return mask & kRead; }

constexpr bool IsWriteEvent(std::uint8_t mask) noexcept {
    return mask & kWrite;
}

constexpr bool IsCloseEvent(std::uint8_t mask) noexcept {
    return mask & static_cast<std::uint8_t>(ReactorEvent::kClose);
}

constexpr void SetReadEvent(ReactorEvent& e) noexcept {}

class Reactor {
public:
    Reactor() = default;
    Reactor(Reactor const&) = delete;
    Reactor& operator=(Reactor const&) = delete;
    Reactor(Reactor&&) = default;
    Reactor& operator=(Reactor&&) = default;

    void Add(int, std::function<void(std::uint8_t)>, std::uint8_t) noexcept;
    void Remove(int) noexcept;
    void Poll() noexcept;

private:
    std::vector<pollfd> fd_data_;
    std::unordered_map<int, std::function<void(std::uint8_t)>> fd_handlers_;
};

}  // namespace minaio::v2
