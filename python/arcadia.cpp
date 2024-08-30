#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "arcadia.h"

namespace py = pybind11;

void init_arcadia(py::module_ &m) {


    py::class_<LunarRayTracer>(m, "LunarRayTracer")

        .def(py::init<RayTracerOptions>(), 
            py::arg("opts") = RayTracerOptions()
        )

        .def("run", &LunarRayTracer::run)

        .def("updateCamera", &LunarRayTracer::updateCamera)
        .def("updateCameraPosition", &LunarRayTracer::updateCameraPosition)
        .def("updateCameraOrientation", &LunarRayTracer::updateCameraOrientation)

        .def("generateImageOptical", &LunarRayTracer::generateImageOptical) 
        .def("generateImageLIDAR", &LunarRayTracer::generateImageLIDAR) 
        .def("generateImageDEM", &LunarRayTracer::generateImageDEM)
        .def("generateImageGCPs", &LunarRayTracer::generateImageGCPs)

        .def("getAltitude", &LunarRayTracer::getAltitude);
}