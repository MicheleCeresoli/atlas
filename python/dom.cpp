
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>

#include "dom.h"

namespace py = pybind11;

void init_dom(py::module_ &m) {


    py::class_<DOM, RasterContainer>(m, "DOM")

        .def(py::init<WorldOptions, ui16_t>(), 
               py::arg("opts") = WorldOptions(), 
               py::arg("nThreads") = 1
          )

        .def(py::init<std::vector<std::string>, ui16_t, bool>(), 
               py::arg("files"), 
               py::arg("nThreads") = 1, 
               py::arg("displayInfo") = false
          )

        .def("getColor", &DOM::getColor);


}