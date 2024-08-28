
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "camera.h"

namespace py = pybind11;

void init_camera(py::module_ &m) {

    py::class_<Camera>(m, "Camera")

        .def(py::init([](std::array<uint, 2> res, std::array<double, 2> fov) {
            return new Camera(res[0], res[1], fov[0], fov[1]);
        }), py::arg("res"), py::arg("fov"))

        .def(py::init<uint, double>(), py::arg("res"), py::arg("fov"))
        
        .def("nPixels", &Camera::nPixels)
        
        .def("get_pos", &Camera::get_pos)
        .def("get_dcm", &Camera::get_dcm)

        .def("set_pos", [](Camera& self, py::array_t<double> pos) {
            
            // Check array dimensions 
            if (pos.ndim() != 1 || pos.shape(0) != 3)
                throw std::runtime_error("Unsupported array dimensions.");
            
            point3 p; 
            std::memcpy(p.e, pos.data(), 3*sizeof(double)); 
            self.set_pos(p); 
        })

        .def("set_pos", [](Camera& self, std::array<double, 3> p) {
            return self.set_pos(point3(p[0], p[1], p[2]));
        })

        .def("set_dcm", [](Camera& self, const dcm& dcm) {
            self.set_dcm(dcm); 
        });

}