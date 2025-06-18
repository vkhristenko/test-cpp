#include "c_frontend/lib/lexer.h"
#include "c_frontend/lib/parser.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

using namespace c_frontend;

TEST(ParserTest, IntMainVarDeclAssignmentReturn) {
    std::string code = "int main() { int a = 0; return a; }";
    Lexer lexer(code);
    std::vector<Token> tokens;
    while (true) {
        auto t = lexer.next();
        ASSERT_TRUE(t.HasValue());
        tokens.push_back(t.Value());
        if (t.Value().type == TokenType::EndOfFile) break;
    }
    Parser parser(tokens);
    auto re_ast = parser.Parse();
    ASSERT_TRUE(re_ast.Ok());

    auto ast = std::move(re_ast).Value();
    ASSERT_TRUE(ast != nullptr);
    auto tu = ast->As<TranslationUnit>();
    ASSERT_TRUE(tu != nullptr);
    ASSERT_EQ(tu->decls.size(), 1u);
    auto func = tu->decls[0]->As<FunctionDecl>();
    ASSERT_TRUE(func != nullptr);
    EXPECT_EQ(func->name, "main");
    EXPECT_EQ(func->body.size(), 2u);
    auto vardecl = func->body[0]->As<VarDecl>();
    ASSERT_TRUE(vardecl != nullptr);
    EXPECT_EQ(vardecl->name, "a");
    auto assign = func->body[1]->As<AssignmentExpr>();
    ASSERT_TRUE(assign != nullptr);
    EXPECT_EQ(assign->lhs, "return");
}
