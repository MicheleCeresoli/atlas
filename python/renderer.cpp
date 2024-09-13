
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>
#include "renderer.h"

namespace py = pybind11;


void init_renderer(py::module_ &m) {

    py::class_<Renderer>(m, "Renderer")

        .def(py::init<RenderingOptions, ui16_t>(), 
            py::arg("opts")=RenderingOptions(), 
            py::arg("nThreads") = 1
        )
        
        .def("getRenderedPixels", &Renderer::getRenderedPixels, 
            py::return_value_policy::reference)

        .def("render", &Renderer::render)
        .def("updateRenderingOptions", &Renderer::updateRenderingOptions)
        .def("getStatus", &Renderer::getStatus);

}