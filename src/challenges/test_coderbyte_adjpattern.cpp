#include <iostream>
#include <string>
#include <string_view>

using namespace std;

string Test(string const& str) {
    size_t pattern_size = str.size() / 2;

    while (pattern_size >= 2) {
        size_t i_p_start = 0;

        string_view str_view{str};
        for (size_t i_p_start = 0; i_p_start < str.size() - pattern_size;
             i_p_start++) {
            auto p_1 = str_view.substr(i_p_start, pattern_size);
            for (size_t j_p_start = i_p_start + pattern_size;
                 j_p_start <= str.size() - pattern_size; j_p_start++) {
                auto p_2 = str_view.substr(j_p_start, pattern_size);
                if (p_1 == p_2) {
                    return string{"yes "} + string{p_1};
                }
            }
        }

        pattern_size--;
    }

    return "no null";
}

int main(int argc, char** argv) {
    std::cout << Test(argv[1]) << std::endl;

    return 0;
}
