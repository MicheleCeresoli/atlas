
#include "camera.h"
#include "utils.h"

#include <cmath>
#include <iostream>

// Constructors 

Camera::Camera(ui32_t width, ui32_t height) : _width(width), _height(height) {}

void Camera::getPixelCoordinates(const ui32_t& id, ui32_t& u, ui32_t& v) const {
    v = id / _width;
    u = id - v*_width;
}

// PINHOLE CAMERA

PinholeCamera::PinholeCamera(ui32_t res, double fov) : Camera(res, res), fov(fov) {
    scale = tan(0.5*fov);
}

 
Ray PinholeCamera::getRay(double u, double v, bool center) const {

    // u is the pixel number along the horizontal coordinate
    // y is the pixel number along the vertical coordinate 

    double x = (1 - 2 * (u + 0.5)/(double)width()) * scale;
    double y = (1 - 2 * (v + 0.5)/(double)height()) * scale;

    // Compute the ray direction in the camera frame, with the Z-axis perpendicular 
    // to the image plane, the X-axis towards the top and the Y-axis 
    // completing the triad.

    vec3 direction = vec3(x, y, 1.0);
    return Ray(_pos, _dcm*direction);

}


// REAL CAMERA (with defocus blur)

RealCamera::RealCamera(ui32_t res, double focalLen, double sensorSize, double fstop) : 
    Camera(res, res), focalLength(focalLen), sensorSize(sensorSize), fstop(fstop) {

    // Compute the Field of View (rad) and scale factor
    fov = 2*atan(sensorSize/(2*focalLength)); 
    scale = tan(0.5*fov); 

    // Compute pixel physical size (mm/px)
    pixSize = sensorSize/res; 

    // Compute aperture size 
    aperture = focalLength/fstop;

}

Ray RealCamera::getRay(double u, double v, bool center) const {

    point3 origin;
    vec3 direction; 

    double x, y;

    if (center) {

        // We shoot a ray as if it were a Pinhole camera
        x = (1 - 2 * (u + 0.5)/(double)width()) * scale;
        y = (1 - 2 * (v + 0.5)/(double)height()) * scale;

        origin = _pos;
        direction = vec3(x, y, 1.0);

    } else {
        
        // We shoot a ray from a random point on the aperture disk to a random point 
        // within the pixel square 
        double r = aperture*sqrt(randomNumber()); 
        double th = 2*PI*randomNumber(); 

        point3 lensPoint(r*cos(th), r*sin(th), 0); 

        // We rotate the point and add it to the camera position after converting from 
        // mm to meters.
        origin = _pos + 1e-3*(_dcm*lensPoint); 
        
        // For the direction, we sample the pixel in a random unit square around the center
        x = -0.5*(sensorSize - pixSize) + (u + randomNumber() - 0.5)*pixSize;
        y = -0.5*(sensorSize - pixSize) + (v + randomNumber() - 0.5)*pixSize;

        point3 pixSample(x, y, focalLength);

        // Compute the direction
        direction = pixSample - lensPoint;
    }

    return Ray(origin, _dcm*direction);

}

