
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "pixel.h"

namespace py = pybind11;

void init_pixel(py::module_ &m) {

    py::class_<PixelData>(m, "PixelData")

        .def(py::init<>())
        .def(py::init<double, point3>())

        .def_readwrite("t", &PixelData::t)
        .def_readwrite("s", &PixelData::s);

    py::class_<RenderedPixel>(m, "RenderedPixel") 

        .def(
            py::init<ui32_t, size_t, double>(), 
            py::arg("id"), py::arg("nSamples"), py::arg("dt")
        )

        .def_readwrite("id", &RenderedPixel::id)
        .def_readwrite("nSamples", &RenderedPixel::nSamples)
        .def_readwrite("data", &RenderedPixel::data)

        .def("updateSamples", &RenderedPixel::updateSamples)
        .def("addPixelData", &RenderedPixel::addPixelData)

        .def("pixResolution", &RenderedPixel::pixResolution)

        .def("pixDistance", &RenderedPixel::pixDistance)
        .def("pixMeanDistance", &RenderedPixel::pixMinDistance)

        .def("pixMinDistance", &RenderedPixel::pixMinDistance)
        .def("pixMaxDistance", &RenderedPixel::pixMaxDistance);

}