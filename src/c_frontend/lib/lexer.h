#pragma once

#include <string>
#include <vector>
#include "core/error.h"

namespace c_frontend {

/// Token types for a minimal C lexer
enum class TokenType {
    Identifier,   /// Variable/function names, e.g. main, foo, bar
    ConstantNumberLiteral, /// Integer or floating-point numeric literals, e.g. 123, 3.14, 2e10
    Keyword,      /// Reserved C keywords, e.g. int, float, return, if, else, while, for, void
    Symbol,       /// Single-character C symbols, e.g. +, -, *, /, =, ;, (, ), {, }
    EndOfFile     /// Indicates the end of input
};

/// Represents a single token
struct Token {
    TokenType type;
    std::string text;
};

/// Error details for lexer errors
class LexerErrorDetails : public core::IErrorDetails {
public:
    explicit LexerErrorDetails(std::string msg) : msg_(std::move(msg)) {}

    std::unique_ptr<core::IErrorDetails> Clone() const noexcept override {
        return std::make_unique<LexerErrorDetails>(msg_);
    }

    std::string to_string() const noexcept override { return msg_; }

    static void const* ClassId() noexcept { return &s_id; }

    bool IsA(void const* const class_id) const noexcept override {
        return class_id == ClassId() || core::IErrorDetails::IsA(class_id);
    }

private:
    std::string msg_;
    static char s_id;
};

/// Minimal lexer interface
class Lexer {
public:
    explicit Lexer(const std::string& input);

    /// Returns the next token or a lexer error
    core::ErrorOr<Token> next();

private:
    std::string input_;
    size_t pos_ = 0;
};

} // namespace c_frontend
