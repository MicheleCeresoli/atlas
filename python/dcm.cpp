
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "dcm.h"

namespace py = pybind11;

void init_dcm(py::module_ &m) {

    py::class_<dcm>(m, "dcm") 
        .def(py::init<>())

        // Ad-hoc constructor that takes a NumPy array 
        .def(py::init([](py::array_t<double> v) {
            // Check array dimensions 
            if (v.ndim() > 2 || v.size() != 9) {
                throw std::runtime_error("Unsupported array dimensions.");
            }

            double data[9]; 
            std::memcpy(data, v.data(), 9*sizeof(double)); 
            return new dcm(data); 

        }))

        .def(py::init([](std::array<double, 9> v) {
            return new dcm(v.begin());
        }))

        .def("det", &dcm::det)
        .def("trace", &dcm::trace)
        .def("transpose", &dcm::transpose)

        .def("__getitem__", [](const dcm& self, int i) {
            return self[i];
        })

        .def("__setitem__", [](dcm& self, int i, double v) {
            self[i] = v; 
        })

        .def("asarray", [](dcm& self) -> py::array_t<double> {
            return py::array_t<double>(
                {3,3}, 
                {3*sizeof(double), sizeof(double)}, 
                &self[0],
                py::cast(&self)
            );
        })

        .def("__repr__", &dcm::toString);

    py::implicitly_convertible<py::array_t<double>, dcm>();
    py::implicitly_convertible<std::array<double, 9>, dcm>();

}