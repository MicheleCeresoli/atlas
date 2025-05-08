#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "atlas.h"

namespace py = pybind11;

void init_atlas(py::module_ &m) {


    py::class_<RayTracer>(m, "RayTracer")

        .def(py::init<RayTracerOptions>(), 
            py::arg("opts") = RayTracerOptions(1)
        )

        .def("run", &RayTracer::run)

        .def("importRayTracedInfo", &RayTracer::importRayTracedInfo)
        .def("exportRayTracedInfo", &RayTracer::exportRayTracedInfo)

        .def("updateRenderingOptions", &RayTracer::updateRenderingOptions)

        .def("updateCamera", &RayTracer::updateCamera)
        .def("updateCameraPosition", &RayTracer::updateCameraPosition)
        .def("updateCameraOrientation", &RayTracer::updateCameraOrientation)

        .def("generateImageOptical", &RayTracer::generateImageOptical, 
            py::arg("filename"), py::arg("type")=CV_8UC1
        )

        .def("generateImageDEM", &RayTracer::generateImageDEM, 
            py::arg("filename"), py::arg("type")=CV_8UC1, py::arg("normalize")=true
        )

        .def("generateDepthMap", &RayTracer::generateDepthMap, 
            py::arg("filename"), py::arg("type")=CV_8UC1
        ) 

        .def("unload", &RayTracer::unload)
        
        .def("generateGCPs", &RayTracer::generateGCPs)
        .def("getAltitude", &RayTracer::getAltitude, 
            py::arg("pos"), py::arg("dcm"), py::arg("maxErr")=-1.0
        );
}