#pragma once

#include "gmock/gmock.h"

namespace minaio::v2::mocks {

struct ReactorMock {
    ReactorMock() noexcept = default;

    MOCK_METHOD(void, Poll, (), noexcept);
}

}
