
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>

#include "dom.h"

namespace py = pybind11;

void init_dom(py::module_ &m) {


    py::class_<DOM, RasterManager>(m, "DOM")

        .def(py::init<WorldOptions, ui32_t>(), 
               py::arg("opts") = WorldOptions(), 
               py::arg("nThreads") = 1
          )

        .def(py::init<std::vector<RasterDescriptor>, ui32_t, bool>(), 
               py::arg("descriptors"), 
               py::arg("nThreads") = 1, 
               py::arg("displayInfo") = false
          )

        .def("getColor", &DOM::getColor);


}