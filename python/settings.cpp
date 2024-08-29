
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "settings.h"

namespace py = pybind11;

void init_settings(py::module_ &m) {

    /* SSAA OPTIONS */
    py::class_<SSAAOptions>(m, "SSAAOptions")
        .def(py::init<>())
        .def_readwrite("nSamples", &SSAAOptions::nSamples)
        .def_readwrite("active", &SSAAOptions::active)
        .def_readwrite("threshold", &SSAAOptions::threshold);


    /* RENDERING OPTIONS */
    py::class_<RenderingOptions>(m, "RenderingOptions")
    
        .def(py::init<>())

        .def_readwrite("ssaa", &RenderingOptions::ssaa)
        .def_readwrite("batchSize", &RenderingOptions::batchSize)
        .def_readwrite("displayInfo", &RenderingOptions::displayInfo);

    /* WORLD OPTIONS */
    py::class_<WorldOptions>(m, "WorldOptions")

        .def(py::init<>())

        .def(py::init<std::string, std::string>(), 
            py::arg("dem") = "", py::arg("dom") = ""
        )

        .def(py::init<std::vector<std::string>, std::string>(), 
            py::arg("dem"), py::arg("dom") = ""
        )

        .def(py::init<std::vector<std::string>, std::vector<std::string>>(), 
            py::arg("dem"), py::arg("dom")
        )

        .def_readwrite("demFiles", &WorldOptions::demFiles)
        .def_readwrite("domFiles", &WorldOptions::domFiles)
        .def_readwrite("displayInfo", &WorldOptions::displayInfo)
        .def_readwrite("rasterUsageThreshold", &WorldOptions::rasterUsageThreshold);

    /* RAYTRACER OPTIONS */
    py::class_<RayTracerOptions>(m, "RayTracerOptions")

        .def(py::init<>())

        .def_readwrite("nThreads", &RayTracerOptions::nThreads)
        .def_readwrite("optsWorld", &RayTracerOptions::optsWorld)
        .def_readwrite("optsRenderer", &RayTracerOptions::optsRenderer);


}