#include <fstream>
#include <iostream>
#include <vector>

int Solve(std::vector<int> values) {}

int main() {
    std::ifstream f{"test.txt"};
    int n;
    f >> n;
    std::vector<int> values;
    for (int i = 0; i < n; i++) {
        int value;
        f >> value;
        values.push_back(value);
    }

    auto const result = Solve(values);

    std::cout << result << std::endl;

    return 0;
}
