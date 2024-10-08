#pragma once

#include <initializer_list>
#include <memory>

namespace mystd {

template<typename T, typename Allocator = std::allocator<T>>
class Vector final {
public:
    /// default
    Vector() noexcept(noexcept(Allocator{}));

    /// default with allocator
    explicit Vector(Allocator const&) noexcept;
    
    /// 
    explicit Vector(std::size_t count, T const& value = T(), Allocator const& = Allocator());

    /// construct from range
    //template<typename Iterator>
    //Vector(Iterator begin, Iterator end) noexcept;

    /// copy
    Vector(Vector const&) noexcept;
    Vector& operator=(Vector const&) noexcept;

    /// move
    Vector(Vector&&) noexcept;
    Vector& operator=(Vector&&) noexcept;

    /// dtor
    ~Vector();

    /// capacity accessors
    std::size_t size() const noexcept;

    bool empty() const noexcept;

    std::size_t capacity() const noexcept;
    
    /// accessors
    /// TODO make them non-throwable
    T const& at(std::size_t) const;
    T& at(std::size_t);

    T const& front() const;
    T& front();

    T const& back() const;
    T& back();

    T const* data() const noexcept;
    T* data() noexcept;

    /// modifiers
    void reserve();

    void resize();

    void push_back(T const&);
    void push_back(T&&);

    void pop_back();

private:
    std::size_t size_ {0};
    std::size_t capacity_ {0};
    T* data_ {nullptr};
    Allocator alloc_;
};

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector() noexcept(noexcept(Allocator{}))
{
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(Allocator const& alloc) noexcept
    : alloc_{alloc}
{
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(std::size_t count, T const& value, Allocator const& alloc)
{
    capacity_ = count * 2;
    size_ = count;
    alloc_ = alloc;
    data_ = alloc_.allocate(capacity_);

    /// construct 
    auto ptr = data_;
    for (std::size_t i=0; i<size_; i++, ptr++) {
        new (ptr) T{value};
    }
}

}
