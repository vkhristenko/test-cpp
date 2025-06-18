#pragma once

#include <sys/socket.h>

#include "core/macros.h"
#include "core/result.h"
#include "fmt/core.h"

namespace miniaio {
namespace details {

//
// TODO may be Result here is an overkill and simple types should be used!
//

inline core::Result<uint32_t, PosixErrno> wrap_socket(int domain, int type,
                                                      int proto) noexcept {
    auto r = socket(domain, type, proto);

    if (r >= 0) {
        return static_cast<uint32_t>(r);
    }

    // TODO we need error details from enums
    return TCPP_MAKE_ERROR(errno);
}

inline core::Result<Error> wrap_close(int fd) noexcept {
    auto r = ::close(fd);

    if (r == 0) {
        return {};
    }

    return TCPP_MAKE_ERROR(errno);
}

template <typename... Ts>
inline core::Result<uint32_t, Error> wrap_fcntl(int fd, int cmd,
                                                Ts... ts) noexcept {
    auto r = ::fcntl(fd, cmd, ts...);

    if (r >= 0) {
        return r;
    }

    return TCPP_MAKE_ERROR(errno);
}

inline core::Result<void, Error> wrap_bind(int fd,
                                           struct socketaddr const* addr,
                                           socklen_t addrlen) noexcept {
    auto r = ::bind(fd, addr, addrlen);

    if (r == 0) {
        return {};
    }

    return TCPP_MAKE_ERROR(errno);
}

}  // namespace details
}  // namespace miniaio
