#pragma once

#include "boost/asio/experimental/channel.hpp"

#include "bext/utils/aliases.h"

namespace bext::utils {

// TODO use concepts here
template<typename T>
using Channel = ::basio::experimental::channel<void(::bsys::error_code, T)>;

}
