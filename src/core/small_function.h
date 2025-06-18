#pragma once

#include "core/small_type_erased_base.h"

namespace core {

template <typename, std::size_t kMaxObjectSize = 64 - 2 * 8>
class SmallFunction;

template <typename R, typename... Args, std::size_t kMaxObjectSize>
class SmallFunction<R(Args...), kMaxObjectSize> final
    : protected SmallTypeErasedBase<kMaxObjectSize> {
public:
    SmallFunction() = default;
    inline SmallFunction(SmallFunction const& r)
        : SmallTypeErasedBase<kMaxObjectSize>(r), caller_{r.caller_} {}
    inline SmallFunction(SmallFunction&& r)
        : SmallTypeErasedBase<kMaxObjectSize>(std::move(r)),
          caller_{r.caller_} {
        r.caller_ = nullptr;
    }
    inline SmallFunction& operator=(SmallFunction const& r) {
        SmallTypeErasedBase<kMaxObjectSize>::operator=(r);
        caller_ = r.caller_;
        return *this;
    }
    inline SmallFunction& operator=(SmallFunction&& r) {
        SmallTypeErasedBase<kMaxObjectSize>::operator=(std::move(r));
        caller_ = r.caller_;
        r.caller_ = nullptr;
        return *this;
    }
    ~SmallFunction() { caller_ = nullptr; }

    template <typename F,
              typename = typename std::enable_if<!std::is_same<
                  typename std::decay<F>::type,
                  SmallFunction<R(Args...), kMaxObjectSize> >::value>::type>
    SmallFunction(F&& f)
        : SmallTypeErasedBase<kMaxObjectSize>(std::forward<F>(f)) {
        struct __Caller {
            static R Apply(char* ptr, Args... args) {
                using DecayedTargetType = typename std::decay<F>::type;
                auto __ptr = reinterpret_cast<DecayedTargetType*>(ptr);
                return __ptr->operator()(std::forward<Args>(args)...);
            }
        };
        caller_ = &__Caller::Apply;
    }

    template <typename F,
              typename = typename std::enable_if<!std::is_same<
                  typename std::decay<F>::type,
                  SmallFunction<R(Args...), kMaxObjectSize> >::value>::type>
    SmallFunction& operator=(F&& f) {
        SmallFunction tmp = std::forward<F>(f);
        *this = std::move(tmp);
        return *this;
    }

    inline R operator()(Args... args) {
        return caller_(const_cast<char*>(this->data_),
                       std::forward<Args>(args)...);
    }

    inline R operator()(Args... args) const {
        return caller_(const_cast<char*>(this->data_),
                       std::forward<Args>(args)...);
    }

    inline explicit operator bool() const noexcept { return HasTarget(); }
    inline bool operator!() const noexcept { return !operator bool(); }
    inline bool HasTarget() const noexcept { return caller_; }

    using SmallTypeErasedBase<kMaxObjectSize>::CanStore;
    using SmallTypeErasedBase<kMaxObjectSize>::GetMaxObjectSize;

private:
    details::FreeOrStaticFunctionPtr<R, char*, Args...> caller_ = nullptr;
};

}  // namespace core
