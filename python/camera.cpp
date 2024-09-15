
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "camera.h"

namespace py = pybind11;


class PyCamera : public Camera {

    public:

        using Camera::Camera; // Inherit constructors

        bool hasAntiAliasing() const override {
            PYBIND11_OVERRIDE_PURE(bool, Camera, hasAntiAliasing,);
        }

        bool hasDefocusBlur() const override {
            PYBIND11_OVERRIDE_PURE(bool, Camera, hasDefocusBlur,);
        }

        Ray getRay(double u, double v, bool center=false) const override {
            PYBIND11_OVERRIDE_PURE(Ray, Camera, getRay, u, v, center);
        }
};

class PyPinholeCamera : public PinholeCamera {

    public: 

        using PinholeCamera::PinholeCamera; // Inherit constructors

        bool hasAntiAliasing() const override {
            PYBIND11_OVERLOAD(bool, PinholeCamera, hasAntiAliasing,);
        }

        bool hasDefocusBlur() const override {
            PYBIND11_OVERLOAD(bool, PinholeCamera, hasDefocusBlur,);
        }

        Ray getRay(double u, double v, bool center=false) const override {
            PYBIND11_OVERLOAD(Ray, PinholeCamera, getRay, u, v, center);
        }
};


class PyRealCamera : public RealCamera {

    public: 

        using RealCamera::RealCamera; // Inherit constructors

        bool hasAntiAliasing() const override {
            PYBIND11_OVERLOAD(bool, RealCamera, hasAntiAliasing,);
        }

        bool hasDefocusBlur() const override {
            PYBIND11_OVERLOAD(bool, RealCamera, hasDefocusBlur,);
        }

        Ray getRay(double u, double v, bool center=false) const override {
            PYBIND11_OVERLOAD(Ray, RealCamera, getRay, u, v, center);
        }
};



void init_camera(py::module_ &m) {

    py::class_<Camera, PyCamera>(m, "Camera")

        .def(py::init<ui32_t, ui32_t>())
        
        .def("width", &Camera::width)
        .def("height", &Camera::height)
        .def("nPixels", &Camera::nPixels)
        
        .def("setDCM", &Camera::setDCM)
        .def("setPos", &Camera::setPos)

        .def("getDCM", &Camera::getDCM)
        .def("getPos", &Camera::getPos)
        
        .def("getRay", &Camera::getRay)
        .def("hasAntiAliasing", &Camera::hasAntiAliasing)
        .def("hasDefocusBlur", &Camera::hasDefocusBlur);


    py::class_<PinholeCamera, Camera, PyPinholeCamera>(m, "PinholeCamera")
        .def(py::init<ui32_t, double>(), py::arg("res"), py::arg("fov"));


    py::class_<RealCamera, Camera, PyRealCamera>(m, "RealCamera")
        .def(py::init<ui32_t, double, double, double>(), 
            py::arg("res"), py::arg("focalLen"), 
            py::arg("sensorSize"), py::arg("fstop"));

}