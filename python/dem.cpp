
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>

#include "dem.h"

namespace py = pybind11;

void init_dem(py::module_ &m) {

    py::class_<DEM>(m, "DEM")

        .def(py::init<std::string, size_t>(), 
             py::arg("filename"), py::arg("nThreads") = 1)

        .def(py::init<std::vector<std::string>, size_t>(), 
             py::arg("files"), py::arg("nThreads") = 1)

        .def("nRasters", &DEM::nRasters)
        .def("getRasterFile", &DEM::getRasterFile)

        .def("loadRaster", &DEM::loadRaster)
        .def("unloadRaster", &DEM::unloadRaster)

        .def("getMeanRadius", &DEM::getMeanRadius)
        .def("getMinAltitude", &DEM::getMinAltitude)
        .def("getMaxAltitude", &DEM::getMaxAltitude)
        
        .def("getResolution", &DEM::getResolution)
        .def("getAltitude", &DEM::getAltitude);


}