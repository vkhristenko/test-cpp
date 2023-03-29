#include <sstream>

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/stl_util.h>
#include <google/protobuf/compiler/plugin.pb.h>

#include "core/macros.h"
#include "fmt/core.h"

using namespace google::protobuf;
using namespace google::protobuf::compiler;

class DummyGenerator : public CodeGenerator {
public:
    bool Generate(const FileDescriptor* file, const std::string& parameter,
                  GeneratorContext* ctx,
                  std::string* error) const override {
        auto name = file->name();
        auto dot_pos = name.find_last_of(".");
        auto name_wo_ext = name.substr(0, dot_pos);
        std::unique_ptr<io::ZeroCopyOutputStream> stream{
            ctx->Open(name_wo_ext + ".dummy")};

        std::stringstream ss;
        for (int i=0; i<file->message_type_count(); i++) {
            auto desc = file->message_type(i);
            auto header = ::fmt::format(
                "Header:\nname = {} Full Name = {}\n", desc->name(), desc->full_name()
            );
            ss << header;

            for (int j=0; j<desc->field_count(); j++) {
                auto f = desc->field(j);
                auto f_str = ::fmt::format(
                    "name = {} type = {}\n",
                    f->name(), f->type_name());
                ss << f_str;
            }
        }

        google::protobuf::io::Printer printer(stream.get(), '$');
        printer.PrintRaw(ss.str());

        return true;
    }
};

int main(int argc, char* argv[]) {
    DummyGenerator generator;
    return PluginMain(argc, argv, &generator);
}
