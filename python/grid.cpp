
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "grid.h"

namespace py = pybind11;

void init_grid(py::module_ &m) {

    py::class_<ScreenGrid>(m, "ScreenGrid") 

        .def(
            py::init<Pixel, ui32_t, ui32_t, const Camera*>(), 
            py::arg("p0"), py::arg("width"), py::arg("height"), py::arg("cam")
        )

        .def("width", &ScreenGrid::width)
        .def("height", &ScreenGrid::height)
        .def("nPixels", &ScreenGrid::nPixels)
        .def("topLeft", &ScreenGrid::topLeft)

        .def("getRayResolution", &ScreenGrid::getRayResolution)
        .def("setRayResolution", &ScreenGrid::setRayResolution)

        .def("isRowAdaptiveRendering", &ScreenGrid::isRowAdaptiveRendering)

        .def("getGPixelCoordinates", &ScreenGrid::getGPixelCoordinates)
        .def("getGPixelGridCoordinates", &ScreenGrid::getGPixelGridCoordinates)

        .def("getGPixelId", py::overload_cast<const ui32_t&, const ui32_t&>(
                &ScreenGrid::getGPixelId, py::const_
        ))

        .def("getGPixelId", py::overload_cast<const ui32_t&>(
            &ScreenGrid::getGPixelId, py::const_
        ))

        .def("getGPixelId", py::overload_cast<const Pixel&>(
            &ScreenGrid::getGPixelId, py::const_
        ))
        
        .def("getGPixelGridId", py::overload_cast<const ui32_t&, const ui32_t&>(
            &ScreenGrid::getGPixelGridId, py::const_
        ))

        .def("getGPixelGridId", py::overload_cast<const Pixel&>(
            &ScreenGrid::getGPixelGridId, py::const_
        ))
        ;

}