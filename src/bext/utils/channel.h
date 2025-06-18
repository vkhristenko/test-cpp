#pragma once

#include "bext/utils/aliases.h"
#include "boost/asio/experimental/channel.hpp"

namespace bext::utils {

// TODO use concepts here
template <typename T>
using Channel = ::basio::experimental::channel<void(::bsys::error_code, T)>;

}  // namespace bext::utils
