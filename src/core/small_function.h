#pragma once

namespace core {

template<typename, std::size_t kMaxObjectSize = 64 - 2*8>
class SmallFunction;

template<typename R, typename... Args, std::size_t kMaxObjectSize>
class SmallTypeErasedBase<R(Args...), kMaxObjectSize> final : protected SmallTypeErasedBase<kMaxObjectSize> {
public:
    SmallFunction() = default;
    SmallFunction(SmallFunction const&);
    SmallFunction(SmallFunction&&);
    SmallFunction& operator=(SmallFunction const&);
    SmallFunction& operator=(SmallFunction&&);
    ~SmallFunction();

    template<
        typename F,
        typename = typename std::enable_if<
            !std::is_same<tyoename std::decay<F>::type, SmallFunction<R(Args...), kMaxObjectSize>>
        >::type
    >
    SmallFunction(F&& f) 
        : SmallTypeErasedBase<kMaxObjectSize>(std::forward<F>(f))
    {
        struct __Caller {
            static R Apply(char* ptr, Args... args) {
                using DecayedTargetType = typename std::decay<F>::type;
                auto __ptr = reinterpret_cast<DecayedTargetType*>(ptr);
                return __ptr->operator()(std::forward<Args>(args)...);
            }
        };
        caller_ = &__Caller::Apply;
    }
    
    template<
        typename F,
        typename = typename std::enable_if<
            !std::is_same<tyoename std::decay<F>::type, SmallFunction<R(Args...), kMaxObjectSize>>
        >::type
    >
    SmallFunction& operator=(F&& f) {
        SmallFunction tmp = std:forward<F>(f);
        *this = std::move(tmp);
        return *this;
    }

    inline R operator()(Args.. args) const {
        return caller_(const_cast<char*>(this->data_), std::forward<Args>(args)...);
    }

    inline explicit operator bool() const noexcept { return HasTarget(); }
    inline operator!() const noexcept { return !operator bool(); }
    inline bool HasTarget() const noexcept { return caller_; }

    using SmallTypeErasedBase<kMaxObjectSize>::CanStore;
    using SmallTypeErasedBase<kMaxObjectSize>::GetMaxObjectSize;

private:
    FreeOrStaticFunctionPtr<R, char*, Args...> caller_ = nullptr;
};

}
