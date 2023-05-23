#include "pybind11/pybind11.h"

int add(int i, int j) {
    return i+j;
}

// TODO name could have been generated from cmake!
PYBIND11_MODULE(pybind11_test, m) {
    m.doc() = "pybind11 example";
    m.def("add", &add, "an add method");
}
