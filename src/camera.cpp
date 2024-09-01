
#include "camera.h"

#include <cmath>
#include <iostream>

// Constructors 

Camera::Camera(uint width, uint height) : _width(width), _height(height) {}

void Camera::getPixelCoordinates(const uint& id, uint& u, uint& v) const {
    v = id / _width;
    u = id - v*_width;
}



// PINHOLE CAMERA

PinholeCamera::PinholeCamera(uint res, double fov) : Camera(res, res), fov(fov) {
    scale = tan(0.5*fov);
}


Ray PinholeCamera::getRay(double u, double v, bool center) const {

    // u is the pixel number along the horizontal coordinate
    // y is the pixel number along the vertical coordinate 

    double x = ((1 - 2 * (v + 0.5)/(double)height())) * scale;
    double y = (2 * (u + 0.5)/(double)width() - 1) * scale;

    // Compute the ray direction in the camera frame, with the Z-axis perpendicular 
    // to the image plane, the X-axis towards the top and the Y-axis 
    // completing the triad.

    vec3 direction = vec3(x, y, 1.0);
    return Ray(_pos, _dcm*direction);

}



// REAL CAMERA (with defocus blur)

RealCamera::RealCamera(uint res, double focalLen, double sensorSize, double fstop) : 
    Camera(res, res), focalLength(focalLen), sensorSize(sensorSize), fstop(fstop) {

    // Compute the Field of View (rad) and scale factor
    fov = 2*atan(sensorSize/(2*focalLength)); 
    scale = tan(0.5*fov); 

    // Compute pixel physical size (mm)
    pixSize = sensorSize/res; 

    // Compute aperture size 
    aperture = focalLength/fstop;

}

Ray RealCamera::getRay(double u, double v, bool center) const {
    
    vec3 direction; 

    if (center) {

        // We shoot a ray as if it were a Pinhole camera
        double x = ((1 - 2 * (v + 0.5)/(double)height())) * scale;
        double y = (2 * (u + 0.5)/(double)width() - 1) * scale;

        direction = vec3(x, y, 1.0);

    } else {
        
        // We shoot a ray from a random point on the aperture disk to a random point 
        // within the pixel square 

        direction = vec3(0.0, 0.0, 0.0);
    }

    /* TODO: finish me!!*/
    return Ray(_pos, _dcm*direction);

}

