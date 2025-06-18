#include "c_frontend/lib/lexer.h"
#include <cctype>
#include <unordered_set>
#include <string>

namespace c_frontend {

char LexerErrorDetails::s_id = 0;

/// Set of C keywords for this minimal lexer
static const std::unordered_set<std::string> kKeywords = {
    "int", "long", "bool", "short", "float", "double", "return", "if", "else", "while", "for", "void"
};

/// Set of valid C single-character symbols for this lexer
static const std::unordered_set<char> kCSymbols = {
    '+', '-', '*', '/', '=', ';', '(', ')', '{', '}', '[', ']', '<', '>', ',', '.', '&', '|', '^', '!', '~', '%', '?', ':', '#'
};

Lexer::Lexer(const std::string& input) : input_(input) {}

/// Helper to check if a character is a valid digit for a given base
static bool is_digit_for_base(char c, int base) {
    if (base == 2) return c == '0' || c == '1';
    if (base == 8) return c >= '0' && c <= '7';
    if (base == 10) return std::isdigit(c);
    if (base == 16) return std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    return false;
}

/// Returns the next token from the input stream or a lexer error
core::ErrorOr<Token> Lexer::next() {
    // Skip whitespace
    while (pos_ < input_.size() && std::isspace(input_[pos_])) {
        ++pos_;
    }
    if (pos_ >= input_.size()) {
        return Token{TokenType::EndOfFile, ""};
    }
    char c = input_[pos_];
    // Identifiers and keywords: [a-zA-Z_][a-zA-Z0-9_]*
    if (std::isalpha(c) || c == '_') {
        size_t start = pos_;
        while (pos_ < input_.size() && (std::isalnum(input_[pos_]) || input_[pos_] == '_')) {
            ++pos_;
        }
        std::string text = input_.substr(start, pos_ - start);
        if (kKeywords.count(text)) {
            return Token{TokenType::Keyword, text};
        } else {
            return Token{TokenType::Identifier, text};
        }
    }
    // Numeric literals: integer or floating-point, with C99 suffixes
    if (std::isdigit(c) || (c == '.' && pos_ + 1 < input_.size() && std::isdigit(input_[pos_ + 1]))) {
        size_t start = pos_;
        bool seen_dot = false;
        bool seen_exp = false;
        int base = 10;
        // Hex, binary, octal
        if (input_[pos_] == '0') {
            if (pos_ + 1 < input_.size() && (input_[pos_ + 1] == 'x' || input_[pos_ + 1] == 'X')) {
                base = 16;
                pos_ += 2;
                while (pos_ < input_.size() && is_digit_for_base(input_[pos_], 16)) ++pos_;
            } else if (pos_ + 1 < input_.size() && (input_[pos_ + 1] == 'b' || input_[pos_ + 1] == 'B')) {
                base = 2;
                pos_ += 2;
                while (pos_ < input_.size() && is_digit_for_base(input_[pos_], 2)) ++pos_;
            } else if (pos_ + 1 < input_.size() && std::isdigit(input_[pos_ + 1])) {
                base = 8;
                ++pos_;
                while (pos_ < input_.size() && is_digit_for_base(input_[pos_], 8)) ++pos_;
            } else {
                // Just '0'
                ++pos_;
            }
        } else {
            // Decimal or floating-point
            while (pos_ < input_.size() && std::isdigit(input_[pos_])) ++pos_;
        }
        // Floating-point part (for decimal only)
        if (pos_ < input_.size() && input_[pos_] == '.') {
            seen_dot = true;
            ++pos_;
            while (pos_ < input_.size() && std::isdigit(input_[pos_])) ++pos_;
        }
        // Exponent part (for decimal/float only)
        if (pos_ < input_.size() && (input_[pos_] == 'e' || input_[pos_] == 'E')) {
            seen_exp = true;
            ++pos_;
            if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) ++pos_;
            while (pos_ < input_.size() && std::isdigit(input_[pos_])) ++pos_;
        }
        // C99 integer and floating-point suffixes (U, L, LL, UL, F, etc.)
        size_t suffix_start = pos_;
        while (pos_ < input_.size()) {
            char ch = input_[pos_];
            if (ch == 'u' || ch == 'U' || ch == 'l' || ch == 'L' || ch == 'f' || ch == 'F') {
                ++pos_;
            } else {
                break;
            }
        }
        return Token{TokenType::ConstantNumberLiteral, input_.substr(start, pos_ - start)};
    }
    // Symbols: only valid C symbols
    if (kCSymbols.count(c)) {
        ++pos_;
        return Token{TokenType::Symbol, std::string(1, c)};
    }
    // Unknown/invalid character: return a lexer error
    std::string msg = "Invalid character in input: '" + std::string(1, c) + "'";
    ++pos_;
    return TCPP_MAKE_ERROR(LexerErrorDetails(msg));
}

} // namespace c_frontend
