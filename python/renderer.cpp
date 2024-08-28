
#include <pybind11/pybind11.h> 
#include <pybind11/stl.h>
#include "renderer.h"

namespace py = pybind11;

void init_renderer(py::module_ &m) {

    py::class_<Renderer>(m, "Renderer")

        .def(py::init<RenderingOptions>(), py::arg("opts")=RenderingOptions())
        .def("render", &Renderer::render);

}