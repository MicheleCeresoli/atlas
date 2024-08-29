
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>

#include "dem.h"

namespace py = pybind11;

void init_dem(py::module_ &m) {

    py::class_<DEM, RasterContainer>(m, "DEM")

        .def(py::init<std::string, RenderingOptions>(), 
             py::arg("filename"), py::arg("opts") = RenderingOptions())

        .def(py::init<std::vector<std::string>, RenderingOptions>(), 
             py::arg("files"), py::arg("opts") = RenderingOptions())

        .def("getMeanRadius", &DEM::getMeanRadius)
        .def("getMinAltitude", &DEM::getMinAltitude)
        .def("getMaxAltitude", &DEM::getMaxAltitude);
        
}