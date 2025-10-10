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
        case CV_32F: dtype = py::dtype::of<float>(); break;
        case CV_64F: dtype = py::dtype::of<double>(); break; 
        default: 
            throw std::runtime_error("Unsupported image type.");
    }

    // Build the matrices shape and size
    std::vector<std::size_t> shape; 
    std::vector<std::size_t> strides;

    if (mat.channels() == 1) {
        // 2D image (rows, cols) 
        shape = {(size_t)mat.rows, (size_t)mat.cols}; 
        strides = {(size_t)mat.step[0], (size_t)mat.step[1]};
    } 
    else {
        // 3D image (rows, cols, channels)
        shape = {(size_t)mat.rows, (size_t)mat.cols, (size_t)mat.channels()};
        strides = {(size_t)mat.step[0], mat.elemSize1() * mat.channels(), mat.elemSize1()};  

    }

    return py::array(dtype, shape, strides, mat.data, py::cast(mat));

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

        .def("createLIDARMap", [](RayTracer& self) -> py::array {
            
            // Generate the image and convert it to a numpy array 
            cv::Mat img = self.createLIDARMap(); 
            return cvMatToNumpy(img);
            
        })
        
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
            py::arg("pos"), py::arg("dcm"), py::arg("dt"), py::arg("maxErr")=-1.0
        )
        
        // Update and retrieve resolutions
        .def("updateMinRayResolution", &RayTracer::updateMinRayResolution)
        .def("updateMaxRayResolution", &RayTracer::updateMaxRayResolution)

        .def("getMinRayResolution", &RayTracer::getMinRayResolution)
        .def("getMaxRayResolution", &RayTracer::getMaxRayResolution)
        
        // Retrieve the world class 
        .def("getWorld", &RayTracer::getWorld, 
            py::return_value_policy::reference_internal
        );

}