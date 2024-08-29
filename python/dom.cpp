
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>

#include "dom.h"

namespace py = pybind11;

void init_dom(py::module_ &m) {

    py::class_<DOM, RasterContainer>(m, "DOM")

        .def(py::init<std::string, RenderingOptions>(), 
             py::arg("filename"), py::arg("opts") = RenderingOptions())

        .def(py::init<std::vector<std::string>, RenderingOptions>(), 
             py::arg("files"), py::arg("opts") = RenderingOptions())

        .def("getColor", &DOM::getColor);

}