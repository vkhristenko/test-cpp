#pragma once
#include <memory>
#include <llvm/IR/Module.h>
#include "c_frontend/lib/parser.h"
#include "core/error.h"
#include "core/unique_ref.h"

namespace llvm {
class LLVMContext;
}

namespace c_frontend {

/// Minimal codegen interface
class Codegen {
public:
    Codegen();
    core::ErrorOr<core::UniqueRef<llvm::Module>> generate(const AstNode& root, llvm::LLVMContext& ctx);
};

/// Codegen error details
class CodegenErrorDetails : public core::IErrorDetails {
public:
    explicit CodegenErrorDetails(std::string msg) : msg_(std::move(msg)) {}
    std::unique_ptr<core::IErrorDetails> Clone() const noexcept override {
        return std::make_unique<CodegenErrorDetails>(msg_);
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

} // namespace c_frontend
