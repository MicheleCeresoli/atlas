
#include <pybind11/pybind11.h> 
#include "world.h"

namespace py = pybind11;

void init_world(py::module_ &m) {

    py::class_<World>(m, "World")

        .def(py::init
            <std::vector<std::string>, std::vector<std::string>, RenderingOptions>(), 
            py::arg("dem_files"), py::arg("dom_files"), py::arg("opts") = RenderingOptions()
        )

        .def("getRayResolution", &World::getRayResolution)
        .def("computeRayResolution", &World::computeRayResolution);


}