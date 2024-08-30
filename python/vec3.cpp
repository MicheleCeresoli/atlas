
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "vec3.h"

namespace py = pybind11;

void init_vec3(py::module_ &m) {

    py::class_<vec3>(m, "vec3")
        .def(py::init<>())
        .def(py::init<double, double, double>())

        // Ad-hoc constructors that takes a NumPy array
        .def(py::init([](py::array_t<double> v) {

            // Check array dimensions
            if (v.ndim() != 1 || v.size() != 3) {
                throw std::runtime_error("Unsupported array dimensions.");
            }

            double data[3]; 
            std::memcpy(data, v.data(), 3*sizeof(double)); 
            return new vec3(data[0], data[1], data[2]); 

        }))

        // Ad-hoc constructor that takes a Python tuple or list 
        .def(py::init([](std::array<double, 3> v) {
            return new vec3(v[0], v[1], v[2]); 
        }))

        .def("norm", &vec3::norm)
        .def("norm2", &vec3::norm2)

        .def("x", &vec3::x)
        .def("y", &vec3::y)
        .def("z", &vec3::z)

        .def("__getitem__", [](const vec3& v, int i) {
            return v[i];
        })
        
        .def("__setitem__", [](vec3& v, int i, double x) {
            v[i] = x;
        })


        .def("asarray", [](const vec3& v) {
            return py::array_t<double>({3}, {sizeof(double)}, {v.e});
        })

        .def("__repr__", &vec3::toString);

    
    py::implicitly_convertible<py::array_t<double>, vec3>();
    py::implicitly_convertible<std::array<double, 3>, vec3>();
}