
#include <pybind11/pybind11.h>
#include "affine.h"

namespace py = pybind11;

PYBIND11_MODULE(pyarcadia, m) {

    m.doc() = "Python/C++ bindings for the arcadia library."; 

    py::class_<Affine>(m, "Affine")
    .def(py::init<>());

}