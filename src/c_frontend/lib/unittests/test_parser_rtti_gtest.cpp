#include "c_frontend/lib/parser.h"
#include <gtest/gtest.h>

using namespace c_frontend;

TEST(ParserRttiTest, IsAAndAsWorkForAllAstNodes) {
    TypeSpecifier ts("int");
    VarDecl vd;
    AssignmentExpr ae;
    FunctionDecl fd;
    TranslationUnit tu;

    AstNode* base_ts = &ts;
    AstNode* base_vd = &vd;
    AstNode* base_ae = &ae;
    AstNode* base_fd = &fd;
    AstNode* base_tu = &tu;

    // TypeSpecifier
    EXPECT_TRUE(base_ts->IsA<TypeSpecifier>());
    EXPECT_FALSE(base_ts->IsA<VarDecl>());
    EXPECT_EQ(base_ts->As<TypeSpecifier>(), &ts);
    EXPECT_EQ(base_ts->As<VarDecl>(), nullptr);

    // VarDecl
    EXPECT_TRUE(base_vd->IsA<VarDecl>());
    EXPECT_TRUE(base_vd->IsA<AstNode>());
    EXPECT_FALSE(base_vd->IsA<AssignmentExpr>());
    EXPECT_EQ(base_vd->As<VarDecl>(), &vd);
    EXPECT_EQ(base_vd->As<AssignmentExpr>(), nullptr);

    // AssignmentExpr
    EXPECT_TRUE(base_ae->IsA<AssignmentExpr>());
    EXPECT_TRUE(base_ae->IsA<AstNode>());
    EXPECT_FALSE(base_ae->IsA<FunctionDecl>());
    EXPECT_EQ(base_ae->As<AssignmentExpr>(), &ae);
    EXPECT_EQ(base_ae->As<FunctionDecl>(), nullptr);

    // FunctionDecl
    EXPECT_TRUE(base_fd->IsA<FunctionDecl>());
    EXPECT_TRUE(base_fd->IsA<AstNode>());
    EXPECT_FALSE(base_fd->IsA<TranslationUnit>());
    EXPECT_EQ(base_fd->As<FunctionDecl>(), &fd);
    EXPECT_EQ(base_fd->As<TranslationUnit>(), nullptr);

    // TranslationUnit
    EXPECT_TRUE(base_tu->IsA<TranslationUnit>());
    EXPECT_TRUE(base_tu->IsA<AstNode>());
    EXPECT_FALSE(base_tu->IsA<TypeSpecifier>());
    EXPECT_EQ(base_tu->As<TranslationUnit>(), &tu);
    EXPECT_EQ(base_tu->As<TypeSpecifier>(), nullptr);
}
