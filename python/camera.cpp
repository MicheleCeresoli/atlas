
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "camera.h"

namespace py = pybind11;

void init_camera(py::module_ &m) {

    py::class_<Camera>(m, "Camera")

        .def(py::init<>())

        .def(py::init([](std::array<uint, 2> res, std::array<double, 2> fov) {
            return new Camera(res[0], res[1], fov[0], fov[1]);
        }), py::arg("res"), py::arg("fov"))

        .def(py::init<uint, double>(), py::arg("res"), py::arg("fov"))
        
        .def("width", &Camera::width)
        .def("height", &Camera::height)
        .def("nPixels", &Camera::nPixels)
        
        .def("get_pos", &Camera::get_pos)
        .def("set_pos", &Camera::set_pos)

        .def("get_dcm", &Camera::get_dcm)
        .def("set_dcm", &Camera::set_dcm);

}