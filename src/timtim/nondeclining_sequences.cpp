#include <iostream>

int main() {
    int seq1[7] = {1, 2, 3, 4, 5, 100, 1000};
    int seq2[10] = {6, 7, 8, 9, 10, 30, 120, 300, 500, 800};
    int seq3[17];
    int N1 = 7;
    int N2 = 10;

    int i = 0;
    int j = 0;
    int k = 0;
    while (i<N1 && j<N2) {
        if (seq1[i] < seq2[j]) {
            seq3[k] = seq1[i];
            i++;
        } else {
            seq3[k] = seq2[j];
            j++;
        }

        k++;
    }

    for (int ii=i; ii<N1; ii++) {
        seq3[k] = seq1[ii];
        k++;
    }

    for (int jj=j; jj<N2; j++) {
        seq3[k] = seq2[jj];
        k++;
    }

    for (int i=0; i<N1+N2; i++) {
        std::cout << seq3[i];
        if (i != N1+N2) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}
