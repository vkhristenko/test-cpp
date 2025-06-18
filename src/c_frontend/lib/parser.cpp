#include "c_frontend/lib/parser.h"
#include <string>

namespace c_frontend {

char ParserErrorDetails::s_id = 0;
char AstNode::s_id = 0;
char TypeSpecifier::s_id = 0;
char VarDecl::s_id = 0;
char AssignmentExpr::s_id = 0;
char FunctionDecl::s_id = 0;
char TranslationUnit::s_id = 0;

namespace {

template <typename T>
core::ErrorOr<T> MakeParserError(const std::string& msg) {
    return TCPP_MAKE_ERROR(ParserErrorDetails(msg));
}

bool IsTypeKeyword(const Token& t) {
    return t.type == TokenType::Keyword &&
           (t.text == "int" || t.text == "float" || t.text == "double" || t.text == "long" || t.text == "short" || t.text == "bool" || t.text == "void");
}

bool IsIdentifier(const Token& t) {
    return t.type == TokenType::Identifier;
}

bool IsConstant(const Token& t) {
    return t.type == TokenType::ConstantNumberLiteral;
}

} // namespace

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

core::ErrorOr<std::unique_ptr<AstNode>> Parser::Parse() {
    auto translation_unit = std::make_unique<TranslationUnit>();
    while (pos_ < tokens_.size() && tokens_[pos_].type != TokenType::EndOfFile) {
        auto func = ParseFunction();
        if (!func.HasValue()) return func.Error();
        translation_unit->decls.push_back(std::move(func).Value());
    }

    return translation_unit;
}

core::ErrorOr<std::unique_ptr<FunctionDecl>> Parser::ParseFunction() {
    if (!(pos_ < tokens_.size() && IsTypeKeyword(tokens_[pos_]))) {
        return MakeParserError<std::unique_ptr<FunctionDecl>>("Expected function return type");
    }
    auto return_type = std::make_unique<TypeSpecifier>(tokens_[pos_++].text);
    if (!(pos_ < tokens_.size() && IsIdentifier(tokens_[pos_]))) {
        return MakeParserError<std::unique_ptr<FunctionDecl>>("Expected function name");
    }
    std::string name = tokens_[pos_++].text;
    if (!(pos_ < tokens_.size() && tokens_[pos_].text == "(")) {
        return MakeParserError<std::unique_ptr<FunctionDecl>>("Expected '('");
    }
    ++pos_;
    if (!(pos_ < tokens_.size() && tokens_[pos_].text == ")")) {
        return MakeParserError<std::unique_ptr<FunctionDecl>>("Expected ')'");
    }
    ++pos_;
    if (!(pos_ < tokens_.size() && tokens_[pos_].text == "{")) {
        return MakeParserError<std::unique_ptr<FunctionDecl>>("Expected '{'");
    }
    ++pos_;
    auto func = std::make_unique<FunctionDecl>();
    func->return_type = std::move(return_type);
    func->name = name;
    // Parse body: var decls, assignments, return
    while (pos_ < tokens_.size() && tokens_[pos_].text != "}") {
        if (IsTypeKeyword(tokens_[pos_])) {
            auto vardecl = ParseVarDecl();
            if (!vardecl.HasValue()) return vardecl.Error();
            func->body.push_back(std::move(vardecl).Value());
        } else if (tokens_[pos_].type == TokenType::Keyword && tokens_[pos_].text == "return") {
            ++pos_;
            auto ret = std::make_unique<AssignmentExpr>();
            auto expr = ParseExpr();
            if (!expr.HasValue()) return expr.Error();
            ret->lhs = "return";
            ret->rhs = std::move(expr).Value();
            func->body.push_back(std::move(ret));
            if (!(pos_ < tokens_.size() && tokens_[pos_].text == ";")) {
                return MakeParserError<std::unique_ptr<FunctionDecl>>("Expected ';' after return");
            }
            ++pos_;
        } else if (IsIdentifier(tokens_[pos_])) {
            auto assign = ParseAssignment();
            if (!assign.HasValue()) return assign.Error();
            func->body.push_back(std::move(assign).Value());
        } else {
            return MakeParserError<std::unique_ptr<FunctionDecl>>("Unexpected token in function body: " + tokens_[pos_].text);
        }
    }
    if (!(pos_ < tokens_.size() && tokens_[pos_].text == "}")) {
        return MakeParserError<std::unique_ptr<FunctionDecl>>("Expected '}' at end of function");
    }
    ++pos_; // skip '}'
    return func;
}

core::ErrorOr<std::unique_ptr<VarDecl>> Parser::ParseVarDecl() {
    if (!(pos_ < tokens_.size() && IsTypeKeyword(tokens_[pos_]))) {
        return MakeParserError<std::unique_ptr<VarDecl>>("Expected variable type");
    }
    auto type = std::make_unique<TypeSpecifier>(tokens_[pos_++].text);
    if (!(pos_ < tokens_.size() && IsIdentifier(tokens_[pos_]))) {
        return MakeParserError<std::unique_ptr<VarDecl>>("Expected variable name");
    }
    std::string name = tokens_[pos_++].text;
    std::unique_ptr<AstNode> init = nullptr;
    if (pos_ < tokens_.size() && tokens_[pos_].text == "=") {
        ++pos_;
        auto expr = ParseExpr();
        if (!expr.HasValue()) return expr.Error();
        init = std::move(expr).Value();
    }
    if (!(pos_ < tokens_.size() && tokens_[pos_].text == ";")) {
        return MakeParserError<std::unique_ptr<VarDecl>>("Expected ';' after variable declaration");
    }
    ++pos_;
    auto decl = std::make_unique<VarDecl>();
    decl->type = std::move(type);
    decl->name = name;
    decl->init_expr = std::move(init);
    return decl;
}

core::ErrorOr<std::unique_ptr<AssignmentExpr>> Parser::ParseAssignment() {
    if (!(pos_ < tokens_.size() && IsIdentifier(tokens_[pos_]))) {
        return MakeParserError<std::unique_ptr<AssignmentExpr>>("Expected identifier on left-hand side of assignment");
    }
    std::string lhs = tokens_[pos_++].text;
    if (!(pos_ < tokens_.size() && tokens_[pos_].text == "=")) {
        return MakeParserError<std::unique_ptr<AssignmentExpr>>("Expected '=' in assignment");
    }
    ++pos_;
    auto rhs = ParseExpr();
    if (!rhs.HasValue()) return rhs.Error();
    if (!(pos_ < tokens_.size() && tokens_[pos_].text == ";")) {
        return MakeParserError<std::unique_ptr<AssignmentExpr>>("Expected ';' after assignment");
    }
    ++pos_;
    auto assign = std::make_unique<AssignmentExpr>();
    assign->lhs = lhs;
    assign->rhs = std::move(rhs).Value();
    return assign;
}

core::ErrorOr<std::unique_ptr<AstNode>> Parser::ParseExpr() {
    if (pos_ < tokens_.size() && IsConstant(tokens_[pos_])) {
        auto lit = std::make_unique<AstNode>(); // Could define a Literal node
        ++pos_;
        return lit;
    } else if (pos_ < tokens_.size() && IsIdentifier(tokens_[pos_])) {
        auto id = std::make_unique<AstNode>(); // Could define an Identifier node
        ++pos_;
        return id;
    } else {
        return MakeParserError<std::unique_ptr<AstNode>>("Unsupported or missing expression");
    }
}

} // namespace c_frontend
