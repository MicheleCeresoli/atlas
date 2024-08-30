
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "vec2.h"

namespace py = pybind11;

void init_vec2(py::module_ &m) {

    py::class_<vec2>(m, "vec2")
        .def(py::init<>())
        .def(py::init<double, double>())

        // Ad-hoc constructors that takes a NumPy array
        .def(py::init([](py::array_t<double> v) {

            // Check array dimensions
            if (v.ndim() != 1 || v.size() != 2) {
                throw std::runtime_error("Unsupported array dimensions.");
            }

            double data[2]; 
            std::memcpy(data, v.data(), 2*sizeof(double)); 
            return new vec2(data[0], data[1]); 

        }))

        // Ad-hoc constructor that takes a Python tuple or list 
        .def(py::init([](std::array<double, 2> v) {
            return new vec2(v[0], v[1]); 
        }))

        .def("norm", &vec2::norm)
        .def("norm2", &vec2::norm2)

        .def("x", &vec2::x)
        .def("y", &vec2::y)

        .def("__getitem__", [](const vec2& v, int i) {
            return v[i];
        })
        
        .def("__setitem__", [](vec2& v, int i, double x) {
            v[i] = x;
        })

        .def("asarray", [](const vec2& v) {
            return py::array_t<double>({2}, {sizeof(double)}, {v.e});
        })

        .def("__repr__", &vec2::toString);

    py::implicitly_convertible<py::array_t<double>, vec2>();
    py::implicitly_convertible<std::array<double, 2>, vec2>();

}