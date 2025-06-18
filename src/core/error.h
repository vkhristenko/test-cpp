#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>

#include "core/result.h"
#include "core/source_context.h"
#include "fmt/core.h"

// Error handling interfaces and utilities for the core module.
// Provides extensible error details, error objects, and helpers.

namespace core {

// Interface for error details used in core error handling.
class IErrorDetails {
public:
    // Virtual destructor for safe polymorphic deletion.
    virtual ~IErrorDetails() = default;

    // Clone the error details object.
    virtual std::unique_ptr<IErrorDetails> Clone() const noexcept = 0;

    // Convert error details to a string for diagnostics.
    virtual std::string to_string() const noexcept = 0;

    // Returns a unique class identifier for RTTI-like checks.
    static void const* ClassId() noexcept { return &s_id; }

    // Checks if the error details is of a given class id.
    virtual bool IsA(void const* const class_id) const noexcept {
        return class_id == ClassId();
    }

    // Type-safe check for error details type.
    template <typename ErrorDetailsT>
    bool IsA() const noexcept {
        static_assert(std::is_base_of_v<IErrorDetails,
                                        typename std::decay_t<ErrorDetailsT>>,
                      "Must derive from IErrorDetails");

        return IsA(ErrorDetailsT::ClassId());
    }

private:
    // Unique static id for this class type.
    static char s_id;
};

// Represents an error with details and source context.
class Error {
public:
    Error() = delete;
    Error(Error const&);
    Error(Error&&) = default;
    Error& operator=(Error const&);
    Error& operator=(Error&&) = default;

    // Construct an error from error details and source location.
    template <typename E, typename = typename std::enable_if<
                              std::is_base_of<IErrorDetails, E>::value>::type>
    Error(E&& e, SourceLocation loc)
        : details_{std::make_unique<E>(std::forward<E>(e))},
          source_context_{std::move(loc)} {}

    // Get the source context of the error.
    inline SourceLocation const& source_context() const noexcept {
        return source_context_;
    }

    // Get the error details object.
    inline IErrorDetails const& details() const noexcept { return *details_; }

    // Convert the error to a string for diagnostics.
    inline std::string to_string() const noexcept {
        return ::fmt::format("Error @{}:{}, details: {}",
                             source_context_.file_name, source_context_.line,
                             details_->to_string());
    }

    // Cast error details to a specific type (unsafe, use IsA<T> first).
    template <typename T>
    T const* As() const noexcept {
        return static_cast<T const*>(details_.get());
    }

    // Type-safe check for error details type.
    template <typename T>
    bool IsA() const noexcept {
        return details_->IsA<T>();
    }

private:
    std::unique_ptr<IErrorDetails> details_;
    SourceLocation source_context_;
};

// Helper for implementing error details with type-safe RTTI.
template <typename Derived, typename Base = IErrorDetails>
class ErrorDetailsHelper : public Base {
public:
    using Base::Base;

    // Checks if the error details is of a given class id or base type.
    bool IsA(void const* const class_id) const noexcept override {
        return class_id == Derived::ClassId() || Base::IsA(class_id);
    }
};

// Error details implementation for string-based errors.
class StringErrorDetails final : public ErrorDetailsHelper<StringErrorDetails> {
public:
    // Construct from a string message.
    explicit StringErrorDetails(std::string const& msg) : msg_{msg} {}

    // Clone the string error details.
    inline std::unique_ptr<IErrorDetails> Clone() const noexcept override {
        return std::make_unique<StringErrorDetails>(msg_);
    }

    // Convert error details to a string.
    inline std::string to_string() const noexcept override { return msg_; }

    // Returns a unique class identifier for RTTI-like checks.
    static void const* ClassId() noexcept { return &s_id; }

private:
    std::string msg_;
    static char s_id;
};

// Alias for a Result type that uses Error for failures.
template <typename T>
using ErrorOr = Result<T, Error>;

}  // namespace core

// Macro to create an Error with current source context.
#define TCPP_MAKE_ERROR(...) \
    ::core::Error { (__VA_ARGS__), TCPP_CURRENT_SOURCE_CONTEXT() }
