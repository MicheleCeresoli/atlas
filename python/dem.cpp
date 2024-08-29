
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>

#include "dem.h"

namespace py = pybind11;

void init_dem(py::module_ &m) {

    py::class_<DEM, RasterContainer>(m, "DEM")

        .def(py::init<std::string, size_t, bool>(), 
             py::arg("filename"), py::arg("nThreads") = 1, py::arg("displayInfo") = false)

        .def(py::init<std::vector<std::string>, size_t, bool>(), 
             py::arg("files"), py::arg("nThreads") = 1, py::arg("displayInfo") = false)

        .def("getMeanRadius", &DEM::getMeanRadius)
        .def("getMinAltitude", &DEM::getMinAltitude)
        .def("getMaxAltitude", &DEM::getMaxAltitude);
        
}