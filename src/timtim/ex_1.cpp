#include <iostream>

int main() {
    int arr[10][10];

    int nrows = 0;
    int ncols = 0;
    int sum_all = 0;
    std::cin >> nrows >> ncols;
    if (nrows > 10 || nrows <= 0) {
        std::cout << 0 << std::endl;
        return 0;
    }

    if (ncols > 10 || ncols <= 0) {
        std::cout << 0 << std::endl;
        return 0;
    }

    for (int irow = 0; irow < nrows; irow++) {
        for (int icol = 0; icol < ncols; icol++) {
            int value;
            std::cin >> value;
            arr[irow][icol] = value;
            sum_all += value;
        }
    }

    double average = (double)sum_all / (nrows * ncols);

    bool found_one = false;
    for (int icol = 0; icol < ncols; icol++) {
        bool found = true;

        for (int irow = 0; irow < nrows; irow++) {
            found = found && arr[irow][icol] > average;
        }

        if (found) {
            if (!found_one) {
                found_one = true;
                std::cout << average;
            }

            std::cout << "  " << icol;
        }
    }

    if (!found_one) {
        std::cout << 0;
    }

    std::cout << std::endl;

    return 0;
}
