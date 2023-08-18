#pragma once

#include <algorithm>
#include <string>
#include <memory>
#include <type_traits>

#include "core/source_context.h"

#include "fmt/core.h"

namespace core {

class IErrorDetails {
public:
    virtual ~IErrorDetails() = default;

    virtual std::unique_ptr<IErrorDetails> Clone() const noexcept = 0;

    virtual std::string to_string() const noexcept = 0;
};

template<typename T>
T const* error_details_cast(IErrorDetails const* ptr) noexcept {
    static_assert(
        std::is_base_of<
            IErrorDetails, 
            typename std::decay<T>::type
        >::value
    );

    return dynamic_cast<T const*>(ptr);
}

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
    Error(SourceContext ctx, E&& e)
        : source_context_{std::move(ctx)}
        , details_{std::make_unique<E>(std::forward<E>(e))}
    {}

    inline SourceContext const& source_context() const noexcept { return source_context_;}

    inline IErrorDetails const& details() const noexcept { return *details_; }

    inline std::string to_string() const noexcept {
        return ::fmt::format("Error @{}:{}, details: {}", 
            source_context_.file_name, 
            source_context_.line,
            details_->to_string()
        );
    }

private:
    SourceContext source_context_;
    std::unique_ptr<IErrorDetails> details_;
};

class StringErrorDetails final : public IErrorDetails {
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

private:
    std::string msg_;
};

}
