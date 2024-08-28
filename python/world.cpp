
#include <pybind11/pybind11.h> 
#include "world.h"

namespace py = pybind11;

void init_world(py::module_ &m) {

    py::class_<World>(m, "World")

        .def(py::init<DEM&>(), py::arg("dem"))

        .def("getRayResolution", &World::getRayResolution)
        .def("computeRayResolution", &World::computeRayResolution);


}