#pragma once

namespace core {

template<std::size_t kMaxObjectSize = 64>
class SmallTypeErasedBase {
public:
    SmallTypeErasedBase() = default;
    SmallTypeErasedBase(SmallTypeErasedBase const&);
    SmallTypeErasedBase(SmallTypeErasedBase&&);
    SmallTypeErasedBase& operator=(SmallTypeErasedBase const&);
    SmallTypeErasedBase& operator=(SmallTypeErasedBase&&);
    ~SmallTypeErasedBase();

    template<typename F>
    constexpr static bool CanStore() noexcept {
        return sizeof(F) <= kMaxObjectSize;
    }

    template<
        typename F,
        typename = typename std::enable_if<
            !std::is_same<typename std::decay<F>::type, SmallTypeErasedBase>::value
            && !std::is_base_of<SmallTypeErasedBase, typename std::decay<F>::type>::value
            && SmallTypeErasedBase::CanStore<F>()
        >::type
    >
    SmallTypeErasedBase(F&& f) {
        using DecayedType = typename std::decay<F>::type;
        new (data_) DecayedType{std::forward<F>(f)};

        struct __Policy {
            static void Apply(char* dest, char* move_src, char const* copy_src) {
                using TargetType = F;
                using DecayedTargetType = typename std::decay<TargetType>::type;

                if (move_src) {
                    auto r = reinterpret_cast<DecayedTargetType*>(move_src);
                    new (dest) DecayedTargetType{std::move(*r)};
                    return ;
                }

                if (copy_src) {
                    auto r = reinterpret_cast<DecayedTargetType const*>(copy_src);
                    new (dest) DecayedTargetType{*r};
                    return ;
                }

                auto __ptr = reinterpret_cast<DecayedTargetType*>(dest);
                __ptr->~DecayedTargetType();
            }
        };

        policy_ = &__Policy::Apply;
    }
    
    template<
        typename F,
        typename = typename std::enable_if<
            !std::is_same<typename std::decay<F>::type, SmallTypeErasedBase>::value
            && !std::is_base_of<SmallTypeErasedBase, typename std::decay<F>::type>::value
            && SmallTypeErasedBase::CanStore<F>()
        >::type
    >
    SmallTypeErasedBase& operator=(F&& f) {
        TCPP_PRINT_HERE();
        SmallTypeErasedBase tmp = std::forward<F>(f);
        *this = std::move(tmp);
        return *this;
    }

    constexpr static auto GetMaxObjectSize() noexcept { return kMaxObjectSize; }

    bool has_value() const noexcept { return policy_; }

private:
    void destroy() {
        TCPP_PRINT_HERE();
        if (policy_) {
            policy_(data_, nullptr, nullptr);
            policy_ = nullptr;
        }
    }

private:
    char data_[kMaxObjectSize];
    FreeOrStaticFunctionPtr<void, char*, char*, char const*> policy_ = nullptr;
};

}
