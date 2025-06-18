#include "c_frontend/lib/codegen.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>

namespace c_frontend {

char CodegenErrorDetails::s_id = 0;

Codegen::Codegen() {}

core::ErrorOr<core::UniqueRef<llvm::Module>> Codegen::generate(const AstNode& root, llvm::LLVMContext& ctx) {
    auto module = new llvm::Module("c_frontend_module", ctx);
    llvm::IRBuilder<> builder(ctx);

    // Only handle TranslationUnit with one FunctionDecl for now
    if (!root.IsA<TranslationUnit>())
        return TCPP_MAKE_ERROR(CodegenErrorDetails("Codegen: Not a translation unit"));
    auto tu = root.As<TranslationUnit>();
    if (tu->decls.empty())
        return TCPP_MAKE_ERROR(CodegenErrorDetails("Codegen: No declarations in translation unit"));
    auto* funcNode = tu->decls[0].get();
    if (!funcNode->IsA<FunctionDecl>())
        return TCPP_MAKE_ERROR(CodegenErrorDetails("Codegen: Not a function declaration"));
    auto* func = funcNode->As<FunctionDecl>();

    // Create function type: int main()
    auto* intTy = llvm::Type::getInt32Ty(ctx);
    auto* funcType = llvm::FunctionType::get(intTy, false);
    auto* llvmFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, func->name, module);

    // Create entry block
    auto* entry = llvm::BasicBlock::Create(ctx, "entry", llvmFunc);
    builder.SetInsertPoint(entry);

    // For: int a = 0;
    int retVal = 0;
    for (const auto& stmt : func->body) {
        if (stmt->IsA<VarDecl>()) {
            // Minimal: ignore for now
        } else if (stmt->IsA<AssignmentExpr>()) {
            // Minimal: ignore for now
        }
        // Handle return statement
        if (stmt->IsA<AssignmentExpr>()) {
            auto* ret = stmt->As<AssignmentExpr>();
            if (ret->lhs == "return" && ret->rhs) {
                // For now, assume rhs is a constant
                retVal = 0; // Default
                // You could walk ret->rhs to get the value
            }
        }
    }

    builder.CreateRet(llvm::ConstantInt::get(intTy, retVal));
    llvm::verifyFunction(*llvmFunc);

    return core::UniqueRef<llvm::Module>(*module);
}

} // namespace c_frontend
