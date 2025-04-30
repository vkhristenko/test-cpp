#include <cstddef>
#include <iostream>
#include <cassert>
#include <deque>
#include <unordered_set>
#include <functional>
#include <memory>
#include <source_location>

enum class AsyncUserState : std::uint8_t {
    kLocked = 0,
    kUnlocked = 1,
    kDetached = 2
    /// TODO we must introduce this to allow to move a value into a holder 
    /// or move a holder itself while the object is locked
    ///kLockedButDetached = 3,
};

enum class AsyncObjectState : std::uint8_t {
    kValid = 0,
    kStartedDestruction = 1,
    kDestroyed = 2
};

struct AsyncScopedBase {
    AsyncScopedBase(AsyncScopedBase* parent)
        : parent_{parent}
    {
    }

    std::unordered_set<AsyncScopedBase*> children_;
    AsyncScopedBase* parent_;
    bool parent_triggered_destroy_ {false};
    AsyncUserState user_state_ = AsyncUserState::kUnlocked;
    AsyncObjectState object_state_ = AsyncObjectState::kValid;

    void Add(AsyncScopedBase& child) {
        children_.insert(&child);
    }

    void Remove(AsyncScopedBase& child) {
        children_.erase(&child);

        if (children_.empty()) {
            switch (user_state_) {
            default: 
                handleIncorrectTransition();
                break;
            case AsyncUserState::kDetached:
            case AsyncUserState::kUnlocked:
                if (parent_triggered_destroy_) {
                    object_state_ = AsyncObjectState::kStartedDestruction;
                    handleStartDestroy();
                }
                break;
            case AsyncUserState::kLocked:
                break ;
            }
        }
    }

    void handleParentStartDestroy() {
        if (parent_triggered_destroy_) {
            return ;
        }

        std::println("handleParentStartDestroy user_state={}", user_state_ == AsyncUserState::kLocked);
        std::println("handleParentStartDestroy user_state={}", user_state_ == AsyncUserState::kUnlocked);
        std::println("handleParentStartDestroy user_state={}", user_state_ == AsyncUserState::kDetached);

        parent_triggered_destroy_ = true;

        switch (user_state_) {
        default: 
            handleIncorrectTransition();
            break;
        case AsyncUserState::kLocked:
            break;
        case AsyncUserState::kUnlocked:
            std::println("handleParentStartDestroy");
            for (auto child : children_) {
                child->handleParentStartDestroy();
            }

            if (children_.empty()) {
                object_state_ = AsyncObjectState::kStartedDestruction;
                handleStartDestroy();
            }
        case AsyncUserState::kDetached:
            /// children have already been notified
            /// just waiting for completion of destruction
            break;
        }
    }

    virtual void handleStartDestroy() = 0;
    virtual void handleIncorrectTransition(std::source_location = std::source_location::current()) = 0;

protected:
    ~AsyncScopedBase() {}
};

template<typename T>
struct AsyncScoped : public AsyncScopedBase {
    template<typename... Args>
    AsyncScoped(AsyncScopedBase* parent, Args&&... args) 
        : AsyncScopedBase(parent)
    {
        new (data_) T(std::forward<Args>(args)...);
        
        parent_->Add(*this);
    }

    bool Lock() {
        /// at least must be in unlocked state
        if (user_state_ != AsyncUserState::kUnlocked) {
            return false;
        }

        if (parent_triggered_destroy_) {
            return false;
        }

        user_state_ = AsyncUserState::kLocked;
        return true;
    }

    void Unlock() {
        if (user_state_ != AsyncUserState::kLocked) {
            handleIncorrectTransition();
            return ;
        }

        user_state_ = AsyncUserState::kUnlocked;
        if (! parent_triggered_destroy_) {
            return ; 
        }
        
        for (auto child : children_) {
            child->handleParentStartDestroy();
        }

        if (children_.empty()) {
            object_state_ = AsyncObjectState::kStartedDestruction;
            handleStartDestroy();
        }
    }

    void Detach() {
        if (user_state_ == AsyncUserState::kLocked or user_state_ == AsyncUserState::kDetached) {
            handleIncorrectTransition();
            return ;
        }
        
        std::println("detach");

        user_state_ = AsyncUserState::kDetached;
        if (! parent_triggered_destroy_) {
            for (auto child : children_) {
                child->handleParentStartDestroy();
            }

            if (children_.empty()) {
                object_state_ = AsyncObjectState::kStartedDestruction;
                handleStartDestroy();
            }

            return ;
        }

        std::println("detach");

        if (! children_.empty()) {
            return ;
        }

        object_state_ = AsyncObjectState::kStartedDestruction;
        handleStartDestroy();
    }

    void handleIncorrectTransition(std::source_location location = std::source_location::current()) override {
        std::println("incorrect transition from line={}", location.line());
        assert(false);
    }

    void handleStartDestroy() override {
        getT().StartDestroy(*this);
    }

    void handleFinishDestroy() {
        object_state_ = AsyncObjectState::kDestroyed;
        getT().~T();
        parent_->Remove(*this);
    }

    char data_[sizeof(T)];

private:
    ~AsyncScoped();

    auto& getT() {
        return *reinterpret_cast<T*>(data_);
    }
};

template<typename T>
struct UniqueAsyncHolder;

template<typename T>
struct LockedPtr {
    T* ptr_;
    UniqueAsyncHolder<T>* holder_;

    LockedPtr(T* ptr, UniqueAsyncHolder<T>& holder)
        : ptr_{ptr}
        , holder_{&holder}
    {
    }

    LockedPtr(std::nullptr_t) 
        : ptr_(nullptr)
        , holder_{nullptr}
    {
    }

    T* operator->() {
        return ptr_;
    }

    operator bool() {
        return ptr_;
    }

    T* get() {
        return ptr_;
    }

    ~LockedPtr();
};

template<typename T>
struct LockedScopePtr {
    AsyncScoped<T>* ptr_;
    UniqueAsyncHolder<T>* holder_;

    LockedScopePtr(AsyncScoped<T>* ptr, UniqueAsyncHolder<T>& holder)
        : ptr_{ptr}
        , holder_{&holder}
    {
    }

    LockedScopePtr(std::nullptr_t) 
        : ptr_(nullptr)
        , holder_{nullptr}
    {
    }

    AsyncScoped<T>* operator->() {
        return ptr_;
    }

    operator bool() {
        return ptr_;
    }

    AsyncScoped<T>* get() {
        return ptr_;
    }

    ~LockedScopePtr();
};

template<typename T>
struct UniqueAsyncHolder {
    UniqueAsyncHolder(AsyncScoped<T>* ptr)
        : ptr_{ptr}
    {
    }

    UniqueAsyncHolder& operator=(std::nullptr_t) {
        if (ptr_) {
            ptr_->Detach();
        }

        ptr_ = nullptr;
    }

    UniqueAsyncHolder(UniqueAsyncHolder<T>&& rhs) 
        : ptr_{rhs.ptr_}
    {
        rhs.ptr_ = nullptr;
    }

    UniqueAsyncHolder<T>& operator=(UniqueAsyncHolder<T>&& rhs) {
        ptr_ = rhs.ptr_;
        rhs.ptr_ = nullptr;
        return *this;
    }
    
    ~UniqueAsyncHolder() {
        if (ptr_) {
            ptr_->Detach();
        }
    }

    T* operator->() {
        return reinterpret_cast<T*>(ptr_->data_);
    }
    
    LockedScopePtr<T> LockScope() {
        if (! ptr_) {
            return nullptr;
        }

        if (auto const ok = ptr_->Lock()) {
            return LockedScopePtr{ptr_, *this};
        }

        return nullptr;
    }

    LockedPtr<T> Lock() {
        if (! ptr_) {
            return nullptr;
        }

        if (auto const ok = ptr_->Lock()) {
            return LockedPtr{reinterpret_cast<T*>(ptr_->data_), *this};
        }

        return nullptr;
    }

    void ReturnLock(T* data) {
        /// TODO protect against return after move!  
        ptr_->Unlock();
    }

    void ReturnLock(AsyncScoped<T>* scope) {
        ptr_->Unlock();
    }

    AsyncScoped<T>* ptr_;
};

template<typename T>
LockedPtr<T>::~LockedPtr() {
    if (holder_) {
        holder_->ReturnLock(ptr_);
    }
}

template<typename T>
LockedScopePtr<T>::~LockedScopePtr() {
    if (holder_) {
        holder_->ReturnLock(ptr_);
    }
}

template<typename U, typename... Args>
UniqueAsyncHolder<U> MakeUniqueAsyncScoped(AsyncScopedBase& parent, Args&&... args) {
    auto async_scoped = new AsyncScoped<U>(&parent, std::forward<Args>(args)...);
    return UniqueAsyncHolder<U>(async_scoped);
}

struct ExecutionContext {
    ExecutionContext()
    {}

    ~ExecutionContext() {}

    std::deque<std::function<void()>> ops;

    template<typename T>
    void Enqueue(T&& op) {
        ops.push_back(std::forward<T>(op));
    }
    
    size_t Poll() {
        auto const num_ops = ops.size();

        for (size_t i=0; i<num_ops; i++) {
            auto op = std::move(ops.front());
            ops.pop_front();
            op();
        }

        return num_ops;
    }
};

struct BlockingScope : public AsyncScopedBase {
    BlockingScope(ExecutionContext& ctx) 
        : AsyncScopedBase(nullptr)
        , ctx_{ctx}
    {  
    }

    ~BlockingScope() {
        parent_triggered_destroy_ = true;
        for (auto child : children_) {
            child->handleParentStartDestroy();
        }

        while (! children_.empty() && object_state_ != AsyncObjectState::kDestroyed) {
            ctx_.Poll();
        }
    }

    void handleStartDestroy() override {
        object_state_ = AsyncObjectState::kDestroyed; 
    }

    void handleIncorrectTransition(std::source_location location = std::source_location::current()) override {
        assert(false);
    }

    ExecutionContext& ctx_;
};

template<typename T>
AsyncScoped<T>* AsAsyncScoped(T* ptr) {
    return reinterpret_cast<AsyncScoped<T>*>(reinterpret_cast<char*>(ptr) - (sizeof(AsyncScoped<T>) - sizeof(T)));
}

struct TestAsyncScope {
    TestAsyncScope(ExecutionContext& ctx, std::string const& msg) 
        : ctx_{ctx}
        , msg_{msg}
    {

    }

    void PrintMe() {
        std::println("msg={}", msg_);
    }

    void StartDestroy(AsyncScoped<TestAsyncScope>& async_scoped) {
        std::println("starting to destroy msg={}", msg_);
        std::println("ready to destroy msg={}", msg_);
        async_scoped.handleFinishDestroy();
#if 0
        ctx_.Enqueue([this, &async_scoped] {
            std::println("ready to destroy msg={}", msg_);
            async_scoped.handleFinishDestroy();
        });
#endif
    }

    ExecutionContext& ctx_;
    std::string msg_;
};

void Test0() {
    ExecutionContext ctx;
    auto ctx_scope = BlockingScope{ctx};

    auto scope0 = MakeUniqueAsyncScoped<TestAsyncScope>(ctx_scope, ctx, "scope0");
    scope0->PrintMe();    
    if (auto ptr = scope0.Lock()) {
        ptr->PrintMe(); 
    }

    std::vector<UniqueAsyncHolder<TestAsyncScope>> xs;
    if (auto scope = scope0.LockScope()) {
        for (size_t i=0; i<10; i++) {
            auto scope01 = MakeUniqueAsyncScoped<TestAsyncScope>(*scope.get(), ctx, "scope0" + std::to_string(i));
            xs.push_back(std::move(scope01));
        }
    }

    std::println("calling context destructor");
}

int main() {
    Test0();

    return 0;
}
