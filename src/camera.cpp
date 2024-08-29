
#include "camera.h"

#include <cmath>
#include <iostream>

// Constructors 

Camera::Camera(uint res_x, uint res_y, double fov_x, double fov_y) :
    _width(res_x), _height(res_y), fov_x(fov_x), fov_y(fov_y), 
    dfov_x(fov_x/(double)res_x), dfov_y(fov_y/(double)res_y) {}

Camera::Camera(uint res, double fov) : Camera(res, res, fov, fov) {} 


// Functions 
void Camera::pixel_coord(const uint& id, uint& u, uint& v) const {
    v = id / _width;
    u = id - v*_width;
}

Ray Camera::get_ray(double u, double v) const {

    // u is the pixel number along the horizontal coordinate
    // y is the pixel number along the vertical coordinate 
    
    double alpha = -0.5*(fov_x - dfov_x) + u*dfov_x; 
    double beta  =  0.5*(fov_y - dfov_y) - v*dfov_y; 

    // Compute the ray direction in the camera frame, with the Z-axis perpendicular 
    // to the image plane, the X-axis towards the top and the Y-axis 
    // completing the triad.

    vec3 direction = vec3(std::tan(beta), std::tan(alpha), 1.0);
    return Ray(center, A*direction);
}
