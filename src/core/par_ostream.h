#pragma once

#include <mutex>

namespace core {

template <typename OStream>
class ParOStream final {
public:
    ParOStream(OStream& os) : os_{os}, lck_{s_mu} {}
    ParOStream(ParOStream const&) = delete;
    ParOStream(ParOStream&&) = delete;
    ParOStream& operator=(ParOStream const&) = delete;
    ParOStream& operator=(ParOStream&&) = delete;

    template <typename T>
    OStream& operator<<(T&& t) {
        os_ << std::forward<T>(t);
        return os_;
    }

private:
    OStream& os_;
    std::lock_guard<std::mutex> lck_;
    inline static std::mutex s_mu{};
};

}  // namespace core
