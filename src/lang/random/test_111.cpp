// Simple CLI argument parsing example for test_111.cpp
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Advanced CLI argument parser with extensible interface
class CLIOptions {
public:
    // OptionHandler is now templated for type safety and flexibility
    template <typename T>
    using OptionHandler = std::function<void(const std::vector<T>& values)>;

    CLIOptions() = default;

    // Add an option with a handler and type conversion
    template <typename T = std::string>
    void add_option(const std::string& name, const std::string& description,
                    bool has_value = false, OptionHandler<T> handler = nullptr,
                    bool multi_value = false) {
        auto converter = [](const std::string& s) -> T {
            if constexpr (std::is_same_v<T, std::string>)
                return s;
            else if constexpr (std::is_same_v<T, int>)
                return std::stoi(s);
            else if constexpr (std::is_same_v<T, double>)
                return std::stod(s);
            else
                static_assert(sizeof(T) == 0,
                              "Unsupported type for CLI option");
        };
        options_[name] = OptionBase{
            description,
            has_value,
            {},
            false,
            nullptr,
            multi_value,
            [handler, converter](const std::vector<std::string>& raw) {
                if (handler) {
                    std::vector<T> vals;
                    for (const auto& s : raw) vals.push_back(converter(s));
                    handler(vals);
                }
            }};
    }

    void parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            auto it = options_.find(arg);
            if (it != options_.end()) {
                it->second.is_set = true;
                std::vector<std::string> vals;
                if (it->second.has_value) {
                    if (it->second.multi_value) {
                        while (i + 1 < argc && argv[i + 1][0] != '-') {
                            vals.push_back(argv[++i]);
                        }
                        it->second.values = vals;
                    } else {
                        if (i + 1 < argc) {
                            vals.push_back(argv[++i]);
                            it->second.values = vals;
                        } else {
                            std::cerr << "Option '" << arg
                                      << "' requires a value!\n";
                        }
                    }
                }
                if (it->second.handler) {
                    it->second.handler(it->second.values);
                }
            } else {
                positional_args_.push_back(arg);
            }
        }
    }

    bool is_set(const std::string& name) const {
        auto it = options_.find(name);
        return it != options_.end() && it->second.is_set;
    }

    // For single-value options
    std::string value(const std::string& name) const {
        auto it = options_.find(name);
        if (it != options_.end() && !it->second.values.empty())
            return it->second.values[0];
        return "";
    }
    // For multi-value options
    std::vector<std::string> values(const std::string& name) const {
        auto it = options_.find(name);
        if (it != options_.end()) return it->second.values;
        return {};
    }

    void print_help(const std::string& prog_name) const {
        std::cout << "Usage: " << prog_name << " [options]\nOptions:\n";
        for (const auto& [name, opt] : options_) {
            std::cout << "  " << name;
            if (opt.has_value) {
                if (opt.multi_value)
                    std::cout << " <value1> [value2 ...]";
                else
                    std::cout << " <value>";
            }
            std::cout << "\t" << opt.description << "\n";
        }
    }

    const std::vector<std::string>& positional() const {
        return positional_args_;
    }

    // Allow user to extend/override option handling with type
    template <typename T>
    void set_handler(const std::string& name, OptionHandler<T> handler) {
        auto it = options_.find(name);
        if (it != options_.end()) {
            auto converter = [](const std::string& s) -> T {
                if constexpr (std::is_same_v<T, std::string>)
                    return s;
                else if constexpr (std::is_same_v<T, int>)
                    return std::stoi(s);
                else if constexpr (std::is_same_v<T, double>)
                    return std::stod(s);
                else
                    static_assert(sizeof(T) == 0,
                                  "Unsupported type for CLI option");
            };
            it->second.handler =
                [handler, converter](const std::vector<std::string>& raw) {
                    if (handler) {
                        std::vector<T> vals;
                        for (const auto& s : raw) vals.push_back(converter(s));
                        handler(vals);
                    }
                };
        }
    }

private:
    struct OptionBase {
        std::string description;
        bool has_value;
        std::vector<std::string> values;
        bool is_set;
        std::function<void(const std::vector<std::string>&)>
            handler;  // stores type-erased handler
        bool multi_value;
    };
    std::map<std::string, OptionBase> options_;
    std::vector<std::string> positional_args_;
};

int main(int argc, char* argv[]) {
    CLIOptions cli;
    cli.add_option("--help", "Show help message");
    cli.add_option("-h", "Show help message");
    cli.add_option<std::string>("--input", "Input file(s)", true, nullptr,
                                true);  // multi-value, string
    cli.add_option<int>(
        "--numbers", "Numbers", true,
        [](const std::vector<int>& vals) {
            std::cout << "[Handler] Numbers: ";
            for (int v : vals) std::cout << v << " ";
            std::cout << "\n";
        },
        true);  // multi-value, int
    cli.add_option("--output", "Output file", true);
    cli.add_option<std::string>("--verbose", "Enable verbose mode", false,
                                [](const std::vector<std::string>&) {
                                    std::cout
                                        << "[Handler] Verbose mode enabled\n";
                                });

    // Example: user extension for --input
    cli.set_handler<std::string>("--input",
                                 [](const std::vector<std::string>& vals) {
                                     std::cout << "[Handler] Input files: ";
                                     for (const auto& v : vals)
                                         std::cout << v << " ";
                                     std::cout << "\n";
                                 });

    cli.parse(argc, argv);

    if (cli.is_set("--help") || cli.is_set("-h")) {
        cli.print_help(argv[0]);
        return 0;
    }

    if (cli.is_set("--verbose")) {
        std::cout << "Verbose mode enabled\n";
    }
    if (cli.is_set("--input")) {
        auto files = cli.values("--input");
        std::cout << "Input: ";
        for (const auto& f : files) std::cout << f << " ";
        std::cout << "\n";
    }
    if (cli.is_set("--output")) {
        std::cout << "Output: " << cli.value("--output") << "\n";
    }
    if (!cli.positional().empty()) {
        std::cout << "Positional arguments:\n";
        for (const auto& arg : cli.positional()) {
            std::cout << "  " << arg << "\n";
        }
    }
    return 0;
}
