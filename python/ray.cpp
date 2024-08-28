
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "ray.h"

namespace py = pybind11;

void init_ray(py::module_ &m) {

    py::class_<Ray>(m, "Ray")

        .def(py::init<point3, vec3>(), py::arg("origin"), py::arg("direction"))

        .def("origin", &Ray::origin)
        .def("direction", &Ray::direction)

        .def("at", &Ray::at)
        .def("minDistance", &Ray::min_distance)
        
        .def("getParameter", [](Ray& self, double r) -> py::array_t<double> {
            double t[2]; 
            self.get_parameter(t, r); 
            return py::array_t<double>({2}, {sizeof(double)}, {t});
        });
}