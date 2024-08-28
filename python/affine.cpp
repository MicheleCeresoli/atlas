
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "affine.h"

namespace py = pybind11;


void init_affine(py::module_ &m) {
    
    py::class_<Affine>(m, "Affine")

        .def(py::init<>())

        // Ad-hoc constructor that takes a NumPy array
        .def(py::init([](py::array_t<double> v) {
            
            // Check array dimensions
            if (v.ndim() > 2 || v.size() > 6) {
                throw std::runtime_error("Unsupported array dimensions.");
            }

            double data[6]; 
            std::memcpy(data, v.data(), 6 * sizeof(double));
            return new Affine(data);

        }))

        // Ad-hoc constructor that takes a tuple 
        .def(py::init([](std::array<double, 6> v) {
            return new Affine(v.begin());
        }))

        .def("det", &Affine::det)
        .def("xoff", &Affine::xoff)
        .def("yoff", &Affine::yoff)


        .def("__getitem__", [](const Affine& self, int i) {
            return self[i];
        })

        .def("__setitem__", [](Affine& self, int i, double v) {
            self[i] = v; 
        })

        // Bind a method that returns a 2D NumPy array
        .def("asarray", [](Affine& self) -> py::array_t<double> {
            return py::array_t<double>(
                {2, 3}, // shape
                {3*sizeof(double), sizeof(double)}, // Stride (size of one element)
                &self[0], // Pointer to the first element
                py::cast(&self) // Reference to this object
            );
        })

        .def("__repr__", &Affine::toString);

}