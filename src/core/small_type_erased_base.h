#pragma once

#include <cstdlib>
#include <utility>

#include "core/macros.h"

namespace core {

namespace details {

template <typename R, typename... Args>
using FreeOrStaticFunctionPtr = R (*)(Args...);

}

template <std::size_t kMaxObjectSize = 64>
class SmallTypeErasedBase {
public:
    SmallTypeErasedBase() = default;
    inline SmallTypeErasedBase(SmallTypeErasedBase const& r)
        : policy_{r.policy_} {
        if (policy_) {
            policy_(data_, nullptr, r.data_);
        }
    }
    inline SmallTypeErasedBase(SmallTypeErasedBase&& r) : policy_{r.policy_} {
        if (policy_) {
            policy_(data_, std::move(r).data_, nullptr);
        }
    }
    inline SmallTypeErasedBase& operator=(SmallTypeErasedBase const& r) {
        this->destruct();
        policy_ = r.policy_;
        if (policy_) {
            policy_(data_, nullptr, data_);
        }
        return *this;
    }
    inline SmallTypeErasedBase& operator=(SmallTypeErasedBase&& r) {
        this->destruct();
        policy_ = r.policy_;
        if (policy_) {
            policy_(data_, std::move(r).data_, nullptr);
        }
        return *this;
    }
    ~SmallTypeErasedBase() { destruct(); }

    template <typename F>
    constexpr static bool CanStore() noexcept {
        return sizeof(F) <= kMaxObjectSize;
    }

    template <typename F,
              typename = typename std::enable_if<
                  !std::is_same<typename std::decay<F>::type,
                                SmallTypeErasedBase>::value &&
                  !std::is_base_of<SmallTypeErasedBase,
                                   typename std::decay<F>::type>::value &&
                  SmallTypeErasedBase::CanStore<F>()>::type>
    SmallTypeErasedBase(F&& f) {
        using DecayedType = typename std::decay<F>::type;
        new (data_) DecayedType{std::forward<F>(f)};

        struct __Policy {
            static void Apply(char* dest, char* move_src,
                              char const* copy_src) {
                using TargetType = F;
                using DecayedTargetType = typename std::decay<TargetType>::type;

                //
                // move ctor
                //
                if (move_src) {
                    auto r = reinterpret_cast<DecayedTargetType*>(move_src);
                    new (dest) DecayedTargetType{std::move(*r)};
                    return;
                }

                //
                // cp ctor
                //
                if (copy_src) {
                    auto r =
                        reinterpret_cast<DecayedTargetType const*>(copy_src);
                    new (dest) DecayedTargetType{*r};
                    return;
                }

                //
                // dtor
                //
                auto __ptr = reinterpret_cast<DecayedTargetType*>(dest);
                __ptr->~DecayedTargetType();
            }
        };

        policy_ = &__Policy::Apply;
    }

    template <typename F,
              typename = typename std::enable_if<
                  !std::is_same<typename std::decay<F>::type,
                                SmallTypeErasedBase>::value &&
                  !std::is_base_of<SmallTypeErasedBase,
                                   typename std::decay<F>::type>::value &&
                  SmallTypeErasedBase::CanStore<F>()>::type>
    SmallTypeErasedBase& operator=(F&& f) {
        SmallTypeErasedBase tmp = std::forward<F>(f);
        *this = std::move(tmp);
        return *this;
    }

    constexpr static auto GetMaxObjectSize() noexcept { return kMaxObjectSize; }

    bool has_value() const noexcept { return policy_; }

private:
    void destruct() {
        if (policy_) {
            policy_(data_, nullptr, nullptr);
            policy_ = nullptr;
        }
    }

protected:
    char data_[kMaxObjectSize];
    details::FreeOrStaticFunctionPtr<void, char*, char*, char const*> policy_ =
        nullptr;
};

}  // namespace core
