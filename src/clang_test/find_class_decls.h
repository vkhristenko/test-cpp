#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>

namespace clang_test {

// Visitor to collect class declarations
class ClassDeclVisitor : public clang::RecursiveASTVisitor<ClassDeclVisitor> {
public:
    bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl);
};

// Collects the names of all class declarations in a translation unit
class FindClassDeclsConsumer : public clang::ASTConsumer {
public:
    void HandleTranslationUnit(clang::ASTContext& context) override;

private:
    ClassDeclVisitor visitor_;
};

/// a factory to create an ASTConsumer
class FindClassDeclsAction : public clang::ASTFrontendAction {
public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance&, llvm::StringRef) override;
};

}  // namespace clang_test
