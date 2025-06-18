#pragma once

#include <utility>
#include <cassert>
#include <memory>

namespace core {

/// UniqueRef: a non-nullable, non-copyable, movable reference wrapper for unique ownership.
/// \tparam T The type of the object to own.
template <typename T>
class UniqueRef {
public:
    /// Construct from lvalue reference (takes ownership, must be valid)
    /// \param obj Reference to the object to own
    UniqueRef(T& obj) noexcept : ptr_(&obj) { assert(ptr_ && "UniqueRef cannot be null"); }

    /// Construct from lvalue reference of convertible type (takes ownership, must be valid)
    template<typename U>
        requires (std::is_convertible_v<U*, T*> && not std::is_same_v<U, T>)
    UniqueRef(U& obj) noexcept : ptr_(&obj) { assert(ptr_ && "UniqueRef cannot be null"); }

    template<typename U>
        requires (std::is_convertible_v<U*, T*> && not std::is_same_v<U, T>)
    UniqueRef(UniqueRef<U>&& other) noexcept : ptr_(&other.release()) {
        assert(ptr_ && "UniqueRef cannot be null");
    }

    /// Move constructor
    /// \param other UniqueRef to move from
    UniqueRef(UniqueRef&& other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }

    /// Move assignment
    /// \param other UniqueRef to move from
    /// \return Reference to this
    UniqueRef& operator=(UniqueRef&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    /// Deleted copy constructor
    UniqueRef(const UniqueRef&) = delete;
    /// Deleted copy assignment
    UniqueRef& operator=(const UniqueRef&) = delete;

    /// Access the referenced object
    /// \return Reference to the owned object
    T& get() noexcept { assert(ptr_); return *ptr_; }
    const T& get() const noexcept { assert(ptr_); return *ptr_; }

    /// Pointer-like access
    /// \return Pointer to the owned object
    T* operator->() noexcept { assert(ptr_); return ptr_; }
    const T* operator->() const noexcept { assert(ptr_); return ptr_; }

    /// Dereference
    /// \return Reference to the owned object
    T& operator*() noexcept { assert(ptr_); return *ptr_; }
    const T& operator*() const noexcept { assert(ptr_); return *ptr_; }

    /// Release ownership (returns pointer, leaves UniqueRef empty)
    /// \return Pointer to the owned object
    T& release() noexcept { T* tmp = ptr_; ptr_ = nullptr; return *tmp; }

    /// Destructor (deletes owned object)
    ~UniqueRef() { delete ptr_; }
private:
    T* ptr_;
};

} // namespace core
