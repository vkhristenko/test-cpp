#include "clang_test/find_class_decls.h"

#include <clang/AST/DeclCXX.h>
#include <llvm/Support/raw_ostream.h>

namespace clang_test {

bool ClassDeclVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* decl) {
    if (decl->isThisDeclarationADefinition() && decl->getIdentifier()) {
        // Print fully qualified name
        llvm::outs() << decl->getQualifiedNameAsString() << "\n";
    }
    return true;
}

void FindClassDeclsConsumer::HandleTranslationUnit(clang::ASTContext& context) {
    visitor_.TraverseDecl(context.getTranslationUnitDecl());
}

std::unique_ptr<clang::ASTConsumer> FindClassDeclsAction::CreateASTConsumer(
    clang::CompilerInstance&, llvm::StringRef) {
    return std::make_unique<FindClassDeclsConsumer>();
}

}  // namespace clang_test
