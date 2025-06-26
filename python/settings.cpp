
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "settings.h"

namespace py = pybind11;

void init_settings(py::module_ &m) {

    /* LOG LEVEL INFORMATION */
    py::enum_<LogLevel>(m, "LogLevel")
        .value("NONE", LogLevel::NONE)
        .value("MINIMAL", LogLevel::MINIMAL)
        .value("DETAILED", LogLevel::DETAILED)
        .export_values();  // Optional: Exports values to the module's namespace

    /* SSAA OPTIONS */
    py::class_<SSAAOptions>(m, "SSAAOptions")
        .def(py::init<>())
        .def_readwrite("nSamples", &SSAAOptions::nSamples)
        .def_readwrite("active", &SSAAOptions::active)
        .def_readwrite("threshold", &SSAAOptions::threshold)
        .def_readwrite("resMultiplier", &SSAAOptions::resMultiplier)
        .def_readwrite("boundarySize", &SSAAOptions::boundarySize);


    /* RENDERING OPTIONS */
    py::class_<RenderingOptions>(m, "RenderingOptions")
    
        .def(py::init<>())

        .def_readwrite("ssaa", &RenderingOptions::ssaa)
        .def_readwrite("batchSize", &RenderingOptions::batchSize)
        .def_readwrite("logLevel", &RenderingOptions::logLevel)
        .def_readwrite("adaptiveTracing", &RenderingOptions::adaptiveTracing);

    /* WORLD OPTIONS */
    py::class_<WorldOptions>(m, "WorldOptions")

        .def(py::init<>())

        .def(py::init<RasterDescriptor, RasterDescriptor>(), 
            py::arg("dem") = RasterDescriptor(), py::arg("dom") = RasterDescriptor()
        )

        .def(py::init<std::vector<RasterDescriptor>, RasterDescriptor>(), 
            py::arg("dem"), py::arg("dom") = RasterDescriptor()
        )

        .def(py::init<RasterDescriptor, std::vector<RasterDescriptor>>(), 
            py::arg("dem") = RasterDescriptor(), py::arg("dom")
        )

        .def(py::init<std::vector<RasterDescriptor>, std::vector<RasterDescriptor>>(), 
            py::arg("dem"), py::arg("dom")
        )

        .def_readwrite("demFiles", &WorldOptions::demFiles)
        .def_readwrite("domFiles", &WorldOptions::domFiles)
        .def_readwrite("logLevel", &WorldOptions::logLevel)
        .def_readwrite("rasterUsageThreshold", &WorldOptions::rasterUsageThreshold)
        .def_readwrite("minRes", &WorldOptions::minRes);

    /* RAYTRACER OPTIONS */
    py::class_<RayTracerOptions>(m, "RayTracerOptions")

        .def(py::init<size_t, LogLevel>(), 
            py::arg("nThreads") = 1, py::arg("logLevel") = LogLevel::MINIMAL
        )

        .def("setLogLevel", &RayTracerOptions::setLogLevel)

        .def_readwrite("logLevel", &RayTracerOptions::logLevel)
        .def_readwrite("nThreads", &RayTracerOptions::nThreads)
        .def_readwrite("optsWorld", &RayTracerOptions::optsWorld)
        .def_readwrite("optsRenderer", &RayTracerOptions::optsRenderer);


}