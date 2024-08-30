
#include "camera.h"

#include <cmath>
#include <iostream>

// Constructors 

Camera::Camera(uint res, double fov) : _width(res), _height(res), fov(fov) {
    scale = tan(0.5*fov);
}


// Functions 
void Camera::pixel_coord(const uint& id, uint& u, uint& v) const {
    v = id / _width;
    u = id - v*_width;
}

Ray Camera::get_ray(double u, double v) const {

    // u is the pixel number along the horizontal coordinate
    // y is the pixel number along the vertical coordinate 

    double x = ((1 - 2 * (v + 0.5)/(double)_height)) * scale;
    double y = (2 * (u + 0.5)/(double)_width - 1) * scale;

    // Compute the ray direction in the camera frame, with the Z-axis perpendicular 
    // to the image plane, the X-axis towards the top and the Y-axis 
    // completing the triad.

    vec3 direction = vec3(x, y, 1.0);
    return Ray(center, A*direction);
}
