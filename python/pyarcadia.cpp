#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_affine(py::module_ &m);  
void init_arcadia(py::module_ &m); 
void init_camera(py::module_ &m); 
void init_dcm(py::module_ &m); 
void init_dem(py::module_ &m);
void init_dom(py::module_ &m);
void init_pixel(py::module_ &m);
void init_raster(py::module_ &m);
void init_ray(py::module_ &m);
void init_renderer(py::module_ &m); 
void init_settings(py::module_ &m);
void init_vec2(py::module_ &m); 
void init_vec3(py::module_ &m); 
void init_world(py::module_ &m);

PYBIND11_MODULE(_arcadia, m) {

    m.doc() = "Python/C++ bindings for the arcadia library."; 

    init_vec2(m); 
    init_vec3(m);
    init_ray(m); 
    init_dcm(m); 

    init_pixel(m); 
    init_camera(m); 

    init_affine(m);
    init_raster(m);  

    init_settings(m); 

    init_dem(m); 
    init_dom(m);

    init_world(m); 
    init_renderer(m);

    init_arcadia(m); 

}