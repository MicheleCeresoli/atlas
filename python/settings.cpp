
#include <pybind11/pybind11.h>
#include "settings.h"

namespace py = pybind11;

void init_settings(py::module_ &m) {

    py::class_<SSAAOptions>(m, "SSAAOptions")
        .def(py::init<>())
        .def_readwrite("nSamples", &SSAAOptions::nSamples)
        .def_readwrite("active", &SSAAOptions::active)
        .def_readwrite("threshold", &SSAAOptions::threshold);

    py::class_<RenderingOptions>(m, "RenderingOptions")
    
        .def(py::init<>())

        .def_readwrite("ssaa", &RenderingOptions::ssaa)
        .def_readwrite("batchSize", &RenderingOptions::batchSize)
        .def_readwrite("nThreads", &RenderingOptions::nThreads)
        .def_readwrite("displayInfo", &RenderingOptions::displayInfo);

}