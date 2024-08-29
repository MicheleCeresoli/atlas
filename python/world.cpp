
#include <pybind11/pybind11.h> 
#include "world.h"

namespace py = pybind11;

void init_world(py::module_ &m) {

    py::class_<World>(m, "World")

        .def(py::init<WorldOptions, uint>(), 
            py::arg("opts") = WorldOptions(), py::arg("nThreads") = 1
        )

        .def("sampleDOM", &World::sampleDOM)

        .def("cleanupDEM", &World::cleanupDEM)
        .def("cleanupDOM", &World::cleanupDOM)

        .def("getRayResolution", &World::getRayResolution)
        .def("computeRayResolution", &World::computeRayResolution);


}