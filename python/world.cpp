
#include <pybind11/pybind11.h> 
#include "world.h"

namespace py = pybind11;

void init_world(py::module_ &m) {

    py::class_<World>(m, "World")

        .def(py::init<WorldOptions, ui32_t>(), 
            py::arg("opts") = WorldOptions(), py::arg("nThreads") = 1
        )

        .def("minRadius", &World::minRadius)
        .def("maxRadius", &World::maxRadius)
        .def("minAltitude", &World::minAltitude)
        .def("maxAltitude", &World::maxAltitude)

        .def("sampleDEM", &World::sampleDEM)
        .def("sampleDOM", &World::sampleDOM)

        .def("cleanup", &World::cleanup)
        .def("cleanupDEM", &World::cleanupDEM)
        .def("cleanupDOM", &World::cleanupDOM)

        .def("getRayResolution", &World::getRayResolution)
        .def("setRayResolution", &World::setRayResolution)
        
        .def("computeRayResolution", &World::computeRayResolution);


}