#include <iostream>

int main() {
    int matrix[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    int averages[3] = {0, 0, 0};
    int N = 3;

    // compute averages
    for (int col=0; col<N; col++) {
        for (int row=0; row<N; row++) {
            averages[col] += matrix[row][col];
        }
        averages[col] /= N;
    }

    // go through all columns and swap columns
    for (int i_col=0; i_col<N; i_col++) {
        // starting from i_col, find the max average to the left
        int max_pos = i_col;
        for (int j_col=i_col+1; j_col<N; j_col++) {
            if (averages[j_col] > averages[max_pos]) {
                max_pos = j_col;
            }
        }

        // swap (only if max was not the i_col)
        if (max_pos != i_col) {
            for (int row=0; row<N; row++) {
                int tmp = matrix[row][i_col];
                matrix[row][i_col] = matrix[row][max_pos];
                matrix[row][max_pos] = tmp;
            }

            // swap averages as well
            int tmp = averages[i_col];
            averages[i_col] = averages[max_pos];
            averages[max_pos] = tmp;
        }
    }

    // print matrix to the screen
    for (int row=0; row<N; row++) {
        if (row != 0) {
            std::cout << std::endl;
        }

        for (int col=0; col<N; col++) {
            std::cout << matrix[row][col];
            
            if (col != N-1) {
                std::cout << " ";
            }
        }
    }
    std::cout << std::endl;
}
