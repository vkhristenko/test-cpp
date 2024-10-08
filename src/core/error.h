#pragma once

#include <algorithm>
#include <string>
#include <memory>
#include <type_traits>

#include "core/result.h"
#include "core/source_context.h"

#include "fmt/core.h"

namespace core {

class IErrorDetails {
public:
    virtual ~IErrorDetails() = default;

    virtual std::unique_ptr<IErrorDetails> Clone() const noexcept = 0;

    virtual std::string to_string() const noexcept = 0;

    static void const* ClassId() noexcept { return &s_id; }

    virtual bool IsA(void const* const class_id) const noexcept {
        return class_id == ClassId();
    }

    template<typename ErrorDetailsT>
    bool IsA() const noexcept {
        static_assert(
            std::is_base_of_v<IErrorDetails, typename std::decay_t<ErrorDetailsT>>,
            "Must derive from IErrorDetails"
        );
    
        return IsA(ErrorDetailsT::ClassId());
    }

private:
    static char s_id;
};

class Error {
public:
    Error() = delete;
    Error(Error const&);
    Error(Error&&) = default;
    Error& operator=(Error const&);
    Error& operator=(Error&&) = default;

    template<
        typename E,
        typename = typename std::enable_if<std::is_base_of<IErrorDetails, E>::value>::type
    >
    Error(E&& e, SourceLocation loc)
        : details_{std::make_unique<E>(std::forward<E>(e))}
        , source_context_{std::move(loc)}
    {}

    inline SourceLocation const& source_context() const noexcept { return source_context_;}

    inline IErrorDetails const& details() const noexcept { return *details_; }

    inline std::string to_string() const noexcept {
        return ::fmt::format("Error @{}:{}, details: {}", 
            source_context_.file_name, 
            source_context_.line,
            details_->to_string()
        );
    }

    template<typename T>
    T const* As() const noexcept {
        return static_cast<T const*>(details_.get());
    }

    template<typename T>
    bool IsA() const noexcept {
        return details_->IsA<T>();
    }

private:
    std::unique_ptr<IErrorDetails> details_;
    SourceLocation source_context_;
};

template<typename Derived, typename Base = IErrorDetails>
class ErrorDetailsHelper : public Base {
public:
    using Base::Base;

    bool IsA(void const* const class_id) const noexcept override {
        return class_id == Derived::ClassId() || Base::IsA(class_id);
    }
};

class StringErrorDetails final : public ErrorDetailsHelper<StringErrorDetails> {
public:
    // TODO ctors taking ownership of string
    explicit StringErrorDetails(std::string const& msg)
        : msg_{msg}
    {}

    inline std::unique_ptr<IErrorDetails> Clone() const noexcept override {
        return std::make_unique<StringErrorDetails>(msg_);
    }

    inline std::string to_string() const noexcept override {
        return msg_;
    }

    static void const* ClassId() noexcept { return &s_id; }

private:
    std::string msg_;

    static char s_id;
};

template<typename T>
using ErrorOr = Result<T, Error>;

}

#define TCPP_MAKE_ERROR(...) \
    ::core::Error{(__VA_ARGS__), TCPP_CURRENT_SOURCE_CONTEXT()}
