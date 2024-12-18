#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "atlas.h"

namespace py = pybind11;

void init_atlas(py::module_ &m) {


    py::class_<LunarRayTracer>(m, "LunarRayTracer")

        .def(py::init<RayTracerOptions>(), 
            py::arg("opts") = RayTracerOptions(1)
        )

        .def("run", &LunarRayTracer::run)

        .def("importRayTracedInfo", &LunarRayTracer::importRayTracedInfo)
        .def("exportRayTracedInfo", &LunarRayTracer::exportRayTracedInfo)

        .def("updateRenderingOptions", &LunarRayTracer::updateRenderingOptions)

        .def("updateCamera", &LunarRayTracer::updateCamera)
        .def("updateCameraPosition", &LunarRayTracer::updateCameraPosition)
        .def("updateCameraOrientation", &LunarRayTracer::updateCameraOrientation)

        .def("generateImageOptical", &LunarRayTracer::generateImageOptical, 
            py::arg("filename"), py::arg("type")=CV_8UC1
        )

        .def("generateImageDEM", &LunarRayTracer::generateImageDEM, 
            py::arg("filename"), py::arg("type")=CV_8UC1, py::arg("normalize")=true
        )

        .def("generateDepthMap", &LunarRayTracer::generateDepthMap, 
            py::arg("filename"), py::arg("type")=CV_8UC1
        ) 

        .def("unload", &LunarRayTracer::unload)
        
        .def("generateGCPs", &LunarRayTracer::generateGCPs)
        .def("getAltitude", &LunarRayTracer::getAltitude);
}