#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "atlas.h"

namespace py = pybind11;

py::array cvMatToNumpy(const cv::Mat& mat) {

    // Determine the numpy array type
    py::dtype dtype; 
    switch (mat.depth()) {
        case CV_8UC1: dtype = py::dtype::of<uint8_t>(); break; 
        case CV_16UC1: dtype = py::dtype::of<uint16_t>(); break;
        default: 
            throw std::runtime_error("Unsupported image type.");
    }

    // Use a shared pointer to ensure memory remains valid 
    auto mat_ptr = std::make_shared<cv::Mat>(mat);

    // py::capsule nesure cv::Mat is alive until Python is done with the data
    return py::array(
        dtype, 
        {mat.rows, mat.cols}, 
        {mat.step[0], mat.step[1]}, 
        mat_ptr->data,
        py::capsule(mat_ptr.get(), [](void* p) {
            delete static_cast<cv::Mat*>(p);
        })
    );

}

void init_atlas(py::module_ &m) {


    py::class_<RayTracer>(m, "RayTracer")

        .def(py::init<RayTracerOptions>(), 
            py::arg("opts") = RayTracerOptions(1)
        )

        .def("run", &RayTracer::run)

        .def("importRayTracedInfo", &RayTracer::importRayTracedInfo)
        .def("exportRayTracedInfo", &RayTracer::exportRayTracedInfo)

        .def("updateRenderingOptions", &RayTracer::updateRenderingOptions)

        .def("updateCamera", &RayTracer::updateCamera)
        .def("updateCameraPosition", &RayTracer::updateCameraPosition)
        .def("updateCameraOrientation", &RayTracer::updateCameraOrientation)

        .def("createImageOptical", [](RayTracer& self, int type) -> py::array {
            
            // Generate the image and convert it to a numpy array
            cv::Mat img = self.createImageOptical(type);
            return cvMatToNumpy(img);

        }, py::arg("type") = CV_8UC1)

        .def("createDepthMap", [](RayTracer& self, int type) -> py::array {

            // Generate the image and convert it to a numpy array 
            cv::Mat img = self.createDepthMap(type); 
            return cvMatToNumpy(img);

        }, py::arg("type") = CV_8UC1)
            
        .def("createImageDEM", [](RayTracer& self, int type, bool normalize) -> py::array {

            // Generate the image and convert it to a numpy array 
            cv::Mat img = self.createImageDEM(type, normalize); 
            return cvMatToNumpy(img);

        }, py::arg("type") = CV_8UC1, py::arg("normalize") = true)
        
        .def("saveImageOptical", &RayTracer::saveImageOptical, 
            py::arg("filename"), py::arg("type") = CV_8UC1
        )

        .def("saveImageDEM", &RayTracer::saveImageDEM, 
            py::arg("filename"), py::arg("type") = CV_8UC1, py::arg("normalize") = true
        )

        .def("saveDepthMap", &RayTracer::saveDepthMap, 
            py::arg("filename"), py::arg("type") = CV_8UC1
        ) 

        .def("unload", &RayTracer::unload)
        
        .def("generateGCPs", &RayTracer::generateGCPs)
        .def("getAltitude", &RayTracer::getAltitude, 
            py::arg("pos"), py::arg("dcm"), py::arg("maxErr")=-1.0
        );
}