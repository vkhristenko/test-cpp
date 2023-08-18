#pragma once

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

class Error {
public:
    Error() = delete;
    Error(Error const&);
    Error(Error&&);
    Error& operator=(Error const&);
    Error& operator=(Error&&);

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

}
