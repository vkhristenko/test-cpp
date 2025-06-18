#include "c_frontend/lib/lexer.h"
#include "c_frontend/lib/parser.h"
#include "c_frontend/lib/codegen.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>

#include <fstream>
#include <iostream>
#include <string>

/// Emits assembly for the given module to the specified file using LLVM API.
/// Returns true on success, false on failure.
bool EmitAssembly(llvm::Module* module, const std::string& filename) {
    LLVMInitializeAArch64TargetInfo();
    LLVMInitializeAArch64Target();
    LLVMInitializeAArch64TargetMC();
    LLVMInitializeAArch64AsmPrinter();
    LLVMInitializeAArch64AsmParser();

    auto target_triple = llvm::sys::getDefaultTargetTriple();
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);
    if (!target) {
        std::cerr << "LLVM Target error: " << error << "\n";
        return false;
    }
    auto cpu = "generic";
    auto features = "";
    llvm::TargetOptions opt;
    std::optional<llvm::Reloc::Model> rm = std::nullopt;
    auto target_machine = target->createTargetMachine(target_triple, cpu, features, opt, rm);
    module->setDataLayout(target_machine->createDataLayout());
    module->setTargetTriple(llvm::Triple(target_triple));

    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    if (ec) {
        std::cerr << "Could not open file: " << ec.message() << "\n";
        return false;
    }
    llvm::legacy::PassManager pass;
    if (target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
        std::cerr << "TargetMachine can't emit a file of this type!\n";
        return false;
    }
    pass.run(*module);
    dest.flush();
    return true;
}

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string output_file;
    bool emit_asm = false;
    bool emit_exe = false;

    // Simple manual CLI parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " <input.c> [-o <output>] [--emit-asm] [--emit-exe]" << std::endl;
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                std::cerr << "Missing argument for " << arg << std::endl;
                return 1;
            }
        } else if (arg == "--emit-asm") {
            emit_asm = true;
        } else if (arg == "--emit-exe") {
            emit_exe = true;
        } else if (input_file.empty()) {
            input_file = arg;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    if (input_file.empty()) {
        std::cerr << "Input file is required.\nUsage: " << argv[0] << " <input.c> [-o <output>] [--emit-asm] [--emit-exe]" << std::endl;
        return 1;
    }
    if (emit_exe && output_file.empty()) {
        output_file = "a.out";
    }

    /// source input
    std::ifstream file(input_file);
    if (!file) {
        std::cerr << "Failed to open file: " << input_file << "\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    /// lexing
    c_frontend::Lexer lexer(source);
    std::vector<c_frontend::Token> tokens;
    for (;;) {
        auto re = lexer.next();
        if (! re.Ok()) {
            auto err = std::move(re).ErrorUnsafe();
            std::cerr << "Lexer error: " << err.to_string() << "\n";
            return 1;
        }

        auto tok = re.ValueUnsafe();
        if (tok.type == c_frontend::TokenType::EndOfFile) break;
        tokens.push_back(tok);
    }

    /// parsing
    c_frontend::Parser parser(tokens);
    auto re_ast = parser.Parse();
    if (! re_ast.Ok())
    {
        auto err = std::move(re_ast).ErrorUnsafe();
        std::cerr << "Parser error: " << err.to_string() << "\n";
        return 1;
    }
    auto ast = std::move(re_ast).ValueUnsafe();
    if (!ast) {
        std::cerr << "Parser returned null AST\n";
        return 1;
    }
    if (! ast->IsA<c_frontend::TranslationUnit>())
    {
        std::cerr << "Parser did not produce a translation unit\n";
        return 1;
    }

    /// code generation
    llvm::LLVMContext ctx;
    c_frontend::Codegen codegen;
    auto re_module = codegen.generate(*ast, ctx);
    if (! re_module.Ok())
    {
        auto err = std::move(re_module).ErrorUnsafe();
        std::cerr << "Codegen error: " << err.to_string() << "\n";
        return 1;
    }
    auto module = std::move(re_module).ValueUnsafe();
    if (emit_exe) {
        std::string asm_file = output_file + ".s";
        if (!EmitAssembly(&module.get(), asm_file)) {
            std::cerr << "Failed to emit assembly to " << asm_file << std::endl;
            return 1;
        }
        std::string cmd = "clang " + asm_file + " -o " + output_file;
        int ret = std::system(cmd.c_str());
        if (ret != 0) {
            std::cerr << "Linking failed with code " << ret << std::endl;
            return 1;
        }
        std::remove(asm_file.c_str());
        std::cout << "Executable written to " << output_file << std::endl;
        return 0;
    } else if (emit_asm) {
        if (output_file.empty()) {
            std::cerr << "Output file required for --emit-asm\n";
            return 1;
        }
        if (!EmitAssembly(&module.get(), output_file)) {
            std::cerr << "Failed to emit assembly to " << output_file << "\n";
            return 1;
        }
        std::cout << "Assembly written to " << output_file << std::endl;
    } else {
        if (!output_file.empty()) {
            std::ofstream ofs(output_file);
            if (!ofs) {
                std::cerr << "Failed to open output file: " << output_file << "\n";
                return 1;
            }
            std::string ir_str;
            llvm::raw_string_ostream ir_stream(ir_str);
            module->print(ir_stream, nullptr);
            ofs << ir_stream.str();
        } else {
            module->print(llvm::outs(), nullptr);
        }
    }
    return 0;
}
