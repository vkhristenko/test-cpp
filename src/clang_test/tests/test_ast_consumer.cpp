#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

#include "clang_test/find_class_decls.h"

static llvm::cl::OptionCategory MyToolCategory("find-class-decls options");
static llvm::cl::extrahelp CommonHelp(
    clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp(
    "This tool finds class declarations in C++ source files.\n");

int main(int argc, const char** argv) {
    auto parser =
        clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!parser) {
        llvm::errs() << "Error creating options parser: " << parser.takeError()
                     << "\n";
        return 1;
    }

    auto& optionsParser = parser.get();

    clang::tooling::ClangTool tool(optionsParser.getCompilations(),
                                   optionsParser.getSourcePathList());
    return tool.run(clang::tooling::newFrontendActionFactory<
                        clang_test::FindClassDeclsAction>()
                        .get());
}
