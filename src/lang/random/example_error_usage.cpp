// Implementation file for core/error.h example usage or tests.
// You can use this as a starting point for unit tests or demonstration code.

#include <iostream>
#include <string>

#include "core/error.h"

int main() {
    // Example: using StringErrorDetails
    core::StringErrorDetails details{"Something went wrong"};
    std::unique_ptr<core::IErrorDetails> clone = details.Clone();
    std::cout << "Details: " << details.to_string() << std::endl;
    std::cout << "Clone: " << clone->to_string() << std::endl;

    // Example: using Error
    core::Error err{core::StringErrorDetails{"File not found"},
                    {__FILE__, __LINE__}};
    std::cout << err.to_string() << std::endl;

    // Example: using ErrorOr
    core::ErrorOr<int> result = 42;
    if (result.Ok()) {
        std::cout << "Value: " << result.Value() << std::endl;
    } else {
        std::cout << "Error: " << result.Error().to_string() << std::endl;
    }

    return 0;
}
