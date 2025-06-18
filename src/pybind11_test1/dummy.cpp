#include "pybind11/pybind11.h"

int add(int i, int j) { return i + j; }

struct Pet {
    Pet(std::string const& name) : name{name} {}
    void set(std::string const& n) { name = n; }
    std::string const& get() const { return name; }

    std::string name;
};

namespace py = pybind11;

// TODO name could have been generated from cmake!
PYBIND11_MODULE(pybind11_test, m) {
    m.doc() = "pybind11 example";
    m.def("add", &add, "an add method");

    py::class_<Pet>(m, "Pet")
        .def(py::init<std::string const&>())
        .def("set", &Pet::set)
        .def("get", &Pet::get);
}
