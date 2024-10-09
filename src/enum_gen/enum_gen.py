import os, sys, argparse, importlib.util

def import_from_path(module_name, file_path):
    spec = importlib.util.spec_from_file_location(module_name, file_path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    return module

def generate_cpp_enum(name, spec):
    str_specs = ""
    for k,v in spec.items():
        str_specs += f"\t{k} = {v},\n"
    str_gen = (
        f"\nenum class {name} {{ \n"
        f"{str_specs}"
        f"}};\n")
    return str_gen

def generate_to_string_view(name, spec):
    str_gen = (
        f"\nconstexpr std::string_view ToStringView({name} enum_value) noexcept {{\n"
        f"\tswitch (enum_value) {{\n"
    )

    for k,v in spec.items():
        str_gen += f"\t\tcase {name}::{k}: return \"{k}\";\n"
    str_gen += f"\t\tdefault: return \"\";\n"
    str_gen += "\t}\n"
    str_gen += "}\n"
    return str_gen

def main():
    parser = argparse.ArgumentParser(
        prog = "enum_gen",
        description = "generatey pretty enum"
    )
    parser.add_argument("-i", "--input", help="input files", type=str, nargs="+")
    parser.add_argument("-o", "--output", help="output file", type=str)
    args = parser.parse_args()
    print(args)

    for finput in args.input:
        print(finput)
        module = import_from_path(finput.split("/")[-1][:-3], finput)
        items = [item for item in dir(module) if (not item.startswith("__") and not item.startswith("_"))]
        print(items)
        str_gened = "#pragma once\n\n#include <string_view>\n"
        for item in items:
            str_gened += generate_cpp_enum(item, module.__dict__[item])
            str_gened += generate_to_string_view(item, module.__dict__[item])

    with open(args.output, "w") as f:
        f.write(f"{str_gened}")

if __name__ == "__main__":
    main()
