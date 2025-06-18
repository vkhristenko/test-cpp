#include "c_frontend/lib/lexer.h"

#include <gtest/gtest.h>

using c_frontend::Lexer;
using c_frontend::TokenType;
using c_frontend::Token;

TEST(LexerTest, IdentifiersAndKeywords) {
    Lexer lexer("int main return foo bar");
    auto t1 = lexer.next();
    ASSERT_TRUE(t1.HasValue());
    EXPECT_EQ(t1.Value().type, TokenType::Keyword);
    EXPECT_EQ(t1.Value().text, "int");
    auto t2 = lexer.next();
    ASSERT_TRUE(t2.HasValue());
    EXPECT_EQ(t2.Value().type, TokenType::Identifier);
    EXPECT_EQ(t2.Value().text, "main");
    auto t3 = lexer.next();
    ASSERT_TRUE(t3.HasValue());
    EXPECT_EQ(t3.Value().type, TokenType::Keyword);
    EXPECT_EQ(t3.Value().text, "return");
    auto t4 = lexer.next();
    ASSERT_TRUE(t4.HasValue());
    EXPECT_EQ(t4.Value().type, TokenType::Identifier);
    EXPECT_EQ(t4.Value().text, "foo");
    auto t5 = lexer.next();
    ASSERT_TRUE(t5.HasValue());
    EXPECT_EQ(t5.Value().type, TokenType::Identifier);
    EXPECT_EQ(t5.Value().text, "bar");
}

TEST(LexerTest, Numbers) {
    Lexer lexer("123 4567");
    auto t1 = lexer.next();
    ASSERT_TRUE(t1.HasValue());
    EXPECT_EQ(t1.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t1.Value().text, "123");
    auto t2 = lexer.next();
    ASSERT_TRUE(t2.HasValue());
    EXPECT_EQ(t2.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t2.Value().text, "4567");
}

TEST(LexerTest, Symbols) {
    Lexer lexer("+ - * / = ;");
    auto t1 = lexer.next();
    ASSERT_TRUE(t1.HasValue());
    EXPECT_EQ(t1.Value().type, TokenType::Symbol);
    EXPECT_EQ(t1.Value().text, "+");
    auto t2 = lexer.next();
    ASSERT_TRUE(t2.HasValue());
    EXPECT_EQ(t2.Value().type, TokenType::Symbol);
    EXPECT_EQ(t2.Value().text, "-");
    auto t3 = lexer.next();
    ASSERT_TRUE(t3.HasValue());
    EXPECT_EQ(t3.Value().type, TokenType::Symbol);
    EXPECT_EQ(t3.Value().text, "*");
    auto t4 = lexer.next();
    ASSERT_TRUE(t4.HasValue());
    EXPECT_EQ(t4.Value().type, TokenType::Symbol);
    EXPECT_EQ(t4.Value().text, "/");
    auto t5 = lexer.next();
    ASSERT_TRUE(t5.HasValue());
    EXPECT_EQ(t5.Value().type, TokenType::Symbol);
    EXPECT_EQ(t5.Value().text, "=");
    auto t6 = lexer.next();
    ASSERT_TRUE(t6.HasValue());
    EXPECT_EQ(t6.Value().type, TokenType::Symbol);
    EXPECT_EQ(t6.Value().text, ";");
}

TEST(LexerTest, MainFunction) {
    auto lexer = Lexer("int main() { return 0; }"); 

    auto t1 = lexer.next();
    ASSERT_TRUE(t1.HasValue());
    EXPECT_EQ(t1.Value().type, TokenType::Keyword);

    auto t2 = lexer.next();
    ASSERT_TRUE(t2.HasValue());
    EXPECT_EQ(t2.Value().type, TokenType::Identifier);

    auto t3 = lexer.next();
    ASSERT_TRUE(t3.HasValue());
    EXPECT_EQ(t3.Value().type, TokenType::Symbol);

    auto t4 = lexer.next();
    ASSERT_TRUE(t4.HasValue());
    EXPECT_EQ(t4.Value().type, TokenType::Symbol);

    auto t5 = lexer.next();
    ASSERT_TRUE(t5.HasValue());
    EXPECT_EQ(t5.Value().type, TokenType::Symbol);

    auto t6 = lexer.next();
    ASSERT_TRUE(t6.HasValue());
    EXPECT_EQ(t6.Value().type, TokenType::Keyword);

    auto t7 = lexer.next();
    ASSERT_TRUE(t7.HasValue());
    EXPECT_EQ(t7.Value().type, TokenType::ConstantNumberLiteral);

    auto t8 = lexer.next();
    ASSERT_TRUE(t8.HasValue());
    EXPECT_EQ(t8.Value().type, TokenType::Symbol);

    auto t9 = lexer.next();
    ASSERT_TRUE(t9.HasValue());
    EXPECT_EQ(t9.Value().type, TokenType::Symbol);

    auto t10 = lexer.next();
    ASSERT_TRUE(t10.HasValue());
    EXPECT_EQ(t10.Value().type, TokenType::EndOfFile);
}

TEST(LexerTest, Error) {
    Lexer lexer("int $foo");
    auto t1 = lexer.next();
    ASSERT_TRUE(t1.HasValue());
    EXPECT_EQ(t1.Value().type, TokenType::Keyword);
    auto t2 = lexer.next();
    ASSERT_FALSE(t2.HasValue()); // Should be an error for '$'
    ASSERT_TRUE(t2.HasError());
    // Optionally, check error details
    auto err = t2.Error();
    EXPECT_EQ(err.details().to_string().find("Invalid character"), 0u);
}

TEST(LexerTest, EndOfFile) {
    Lexer lexer("int");
    auto t1 = lexer.next();
    ASSERT_TRUE(t1.HasValue());
    EXPECT_EQ(t1.Value().type, TokenType::Keyword);
    auto t2 = lexer.next();
    ASSERT_TRUE(t2.HasValue());
    EXPECT_EQ(t2.Value().type, TokenType::EndOfFile);
    // Repeated calls should continue to return EndOfFile
    auto t3 = lexer.next();
    ASSERT_TRUE(t3.HasValue());
    EXPECT_EQ(t3.Value().type, TokenType::EndOfFile);
}

TEST(LexerTest, FloatKeywordAndLiterals) {
    Lexer lexer("float 3.14 .5 2e10 1.0f 6.022E23 0.1L");
    auto t1 = lexer.next();
    ASSERT_TRUE(t1.HasValue());
    EXPECT_EQ(t1.Value().type, TokenType::Keyword);
    EXPECT_EQ(t1.Value().text, "float");
    auto t2 = lexer.next();
    ASSERT_TRUE(t2.HasValue());
    EXPECT_EQ(t2.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t2.Value().text, "3.14");
    auto t3 = lexer.next();
    ASSERT_TRUE(t3.HasValue());
    EXPECT_EQ(t3.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t3.Value().text, ".5");
    auto t4 = lexer.next();
    ASSERT_TRUE(t4.HasValue());
    EXPECT_EQ(t4.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t4.Value().text, "2e10");
    auto t5 = lexer.next();
    ASSERT_TRUE(t5.HasValue());
    EXPECT_EQ(t5.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t5.Value().text, "1.0f");
    auto t6 = lexer.next();
    ASSERT_TRUE(t6.HasValue());
    EXPECT_EQ(t6.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t6.Value().text, "6.022E23");
    auto t7 = lexer.next();
    ASSERT_TRUE(t7.HasValue());
    EXPECT_EQ(t7.Value().type, TokenType::ConstantNumberLiteral);
    EXPECT_EQ(t7.Value().text, "0.1L");
}

TEST(LexerTest, NumericConstantsWithSuffixes) {
    Lexer lexer("42U 123u 0xFFUL 100L 77LL 3.14F 2.71f 1.0 1.0L 1.0l 0x1A3uLL 0b1010U");
    std::vector<std::string> expected = {
        "42U", "123u", "0xFFUL", "100L", "77LL", "3.14F", "2.71f", "1.0", "1.0L", "1.0l", "0x1A3uLL", "0b1010U"
    };
    for (const auto& exp : expected) {
        auto t = lexer.next();
        ASSERT_TRUE(t.HasValue());
        EXPECT_EQ(t.Value().type, TokenType::ConstantNumberLiteral);
        EXPECT_EQ(t.Value().text, exp);
    }
}
