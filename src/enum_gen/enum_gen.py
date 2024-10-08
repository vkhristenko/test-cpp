import os, sys, argparse, importlib

def import_from_path(module_name, file_path):
    spec = importlib.util.spec_from_file_location(module_name, file_path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    return module

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
        print([item for item in dir(module) if (not item.startswith("__") and not item.starswith("_"))])

    with open(args.output, "w") as f:
        f.write("void func() {}")

if __name__ == "__main__":
    main()
