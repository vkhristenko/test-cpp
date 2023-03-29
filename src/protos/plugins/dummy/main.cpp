#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/plugin.h>

#include "core/macros.h"
#include "fmt/core.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

class DummyGenerator : public CodeGenerator {
public:
    bool Generate(const FileDescriptor* file, const std::string& parameter,
                  GeneratorContext* generator_context,
                  std::string* error) const override {
        ::fmt::print("{}", TCPP_PRETTY_FUNCTION());
        return true;
    }
};

int main(int argc, char* argv[]) {
    DummyGenerator generator;
    return PluginMain(argc, argv, &generator);
}
