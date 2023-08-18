#pragma once

#include <typeindex>

#include "core/small_type_erased_base.h"

namespace core {

//
// TODO
//

/*
template<std::size_t kMaxObjectSize = 64>
class SmallAny final : protected SmallTypeErasedBase<kMaxObjectSize> {
public:
    // ctors, dtors
    std::type_index const& type() const noexcept { return type_; }

private:
    std::type_index type_ = typeid(void);
};

template<
    typename T,
    std::size_t kMaxObjectSize,
    typename = typename std::enable_if<
        std::is_const<T>::value
    >::type
>
std::decay<T> const* small_any_cast(SmallAny<kMaxObjectSize> const* ptr) noexcept {
    if (!ptr) {
        return nullptr;
    }

    auto const ti = std::type_index{typeid(T)};
    if (ti != ptr->type()) {
        return nullptr;
    }

    return reinterpret_cast<typename std::decay<T>::type const*>(ptr->data());
}
*/

}
