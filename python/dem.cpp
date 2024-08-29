
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>

#include "dem.h"

namespace py = pybind11;

void init_dem(py::module_ &m) {

    py::class_<DEM, RasterContainer>(m, "DEM")

        .def(py::init<WorldOptions, uint>(), 
               py::arg("opts") = WorldOptions(), 
               py::arg("nThreads") = 1
          )

        .def(py::init<std::vector<std::string>, uint, bool>(), 
               py::arg("files"), 
               py::arg("nThreads") = 1, 
               py::arg("displayInfo") = false
          )

        .def("minAltitude", &DEM::minAltitude)
        .def("maxAltitude", &DEM::maxAltitude)

        .def("minRadius", &DEM::minRadius)
        .def("maxRadius", &DEM::maxRadius)
        .def("meanRadius", &DEM::meanRadius);

        
}