#pragma once
#include <memory>
#include <vector>
#include <string>
#include "c_frontend/lib/lexer.h"
#include "core/error.h"

namespace c_frontend {

/// AST node base
struct AstNode {
    virtual ~AstNode() = default;

    static void const* ClassId() noexcept { return &s_id; }

    virtual bool IsA(void const* class_id) const noexcept {
        return class_id == ClassId();
    }

    template <typename NodeT>
    bool IsA() const noexcept {
        static_assert(std::is_base_of_v<AstNode, std::decay_t<NodeT>>, "NodeT must derive from AstNode");
        return IsA(NodeT::ClassId());
    }

    template<typename NodeT>
    NodeT const* As() const noexcept {
        static_assert(std::is_base_of_v<AstNode, std::decay_t<NodeT>>, "NodeT must derive from AstNode");
        if (!IsA<NodeT>()) {
            return nullptr;
        }

        return static_cast<NodeT const*>(this);
    }

    template<typename NodeT>
    NodeT* As() noexcept {
        static_assert(std::is_base_of_v<AstNode, std::decay_t<NodeT>>, "NodeT must derive from AstNode");
        if (!IsA<NodeT>()) {
            return nullptr;
        }

        return static_cast<NodeT*>(this);
    }

private:
    static char s_id;
};

// Helper for implementing error details with type-safe RTTI.
template <typename Derived, typename Base = AstNode>
struct AstNodeHelper : public Base {
public:
    using Base::Base;

    // Checks if the error details is of a given class id or base type.
    bool IsA(void const* const class_id) const noexcept override {
        return class_id == Derived::ClassId() || Base::IsA(class_id);
    }
};

/// Type specifier node (e.g., int, float)
struct TypeSpecifier : public AstNodeHelper<TypeSpecifier> {
    std::string type_name;
    explicit TypeSpecifier(const std::string& t) : type_name(t) {}
    static void const* ClassId() noexcept { return &s_id; }
private:
    static char s_id;
};

/// Variable declaration node (e.g., int a = 0;)
struct VarDecl : public AstNodeHelper<VarDecl> {
    std::unique_ptr<TypeSpecifier> type;
    std::string name;
    std::unique_ptr<AstNode> init_expr; // can be nullptr
    static void const* ClassId() noexcept { return &s_id; }
private:
    static char s_id;
};

/// Assignment expression node (e.g., a = 0)
struct AssignmentExpr : public AstNodeHelper<AssignmentExpr> {
    std::string lhs;
    std::unique_ptr<AstNode> rhs;
    static void const* ClassId() noexcept { return &s_id; }
private:
    static char s_id;
};

/// Function declaration/definition node
struct FunctionDecl : public AstNodeHelper<FunctionDecl> {
    std::unique_ptr<TypeSpecifier> return_type;
    std::string name;
    std::vector<std::unique_ptr<VarDecl>> params;
    std::vector<std::unique_ptr<AstNode>> body;
    static void const* ClassId() noexcept { return &s_id; }
private:
    static char s_id;
};

/// Translation unit (root node)
struct TranslationUnit : public AstNodeHelper<TranslationUnit> {
    std::vector<std::unique_ptr<AstNode>> decls;
    static void const* ClassId() noexcept { return &s_id; }
private:
    static char s_id;
};

/// Error details for parser errors
class ParserErrorDetails : public core::IErrorDetails {
public:
    explicit ParserErrorDetails(std::string msg) : msg_(std::move(msg)) {}
    std::unique_ptr<core::IErrorDetails> Clone() const noexcept override {
        return std::make_unique<ParserErrorDetails>(msg_);
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

/// Minimal parser interface
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    core::ErrorOr<std::unique_ptr<AstNode>> Parse();
private:
    const std::vector<Token>& tokens_;
    size_t pos_ = 0;
    core::ErrorOr<std::unique_ptr<FunctionDecl>> ParseFunction();
    core::ErrorOr<std::unique_ptr<VarDecl>> ParseVarDecl();
    core::ErrorOr<std::unique_ptr<AssignmentExpr>> ParseAssignment();
    core::ErrorOr<std::unique_ptr<AstNode>> ParseExpr();
};

} // namespace c_frontend
