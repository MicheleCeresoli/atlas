#include <pybind11/pybind11.h>

void init_affine(pybind11::module_ &m);  
void init_arcadia(pybind11::module_ &m); 
void init_camera(pybind11::module_ &m); 
void init_dcm(pybind11::module_ &m); 
void init_dem(pybind11::module_ &m);
void init_dom(pybind11::module_ &m);
void init_pixel(pybind11::module_ &m);
void init_raster(pybind11::module_ &m);
void init_ray(pybind11::module_ &m);
void init_renderer(pybind11::module_ &m); 
void init_settings(pybind11::module_ &m);
void init_vec2(pybind11::module_ &m); 
void init_vec3(pybind11::module_ &m); 
void init_world(pybind11::module_ &m);

PYBIND11_MODULE(pyarcadia, m) {

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