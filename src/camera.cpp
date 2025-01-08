
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

PinholeCamera::PinholeCamera(ui32_t res, double fov) : 
    PinholeCamera(res, res, fov, fov) {}; 

PinholeCamera::PinholeCamera(ui32_t width, ui32_t height, double fov_x, double fov_y) : 
    Camera(width, height) 
{
    fov[0] = fov_x; 
    fov[1] = fov_y; 

    scale[0] = tan(0.5*fov[0]);
    scale[1] = tan(0.5*fov[1]);

}

 
Ray PinholeCamera::getRay(double u, double v, bool center) const {

    // u is the pixel number along the horizontal coordinate
    // y is the pixel number along the vertical coordinate 
    double x = (2 * (u + 0.5)/(double)width() - 1) * scale[0];
    double y = (2 * (v + 0.5)/(double)height() - 1) * scale[1];

    // Compute the ray direction in the camera frame, with the Z-axis perpendicular 
    // to the image plane, the X-axis towards the top and the Y-axis 
    // completing the triad.

    vec3 direction = vec3(x, y, 1.0);
    return Ray(_pos, _dcm*direction);

}


// REAL CAMERA (with defocus blur)

RealCamera::RealCamera(ui32_t res, double focalLen, double sensSize, double fstop) : 
    RealCamera(res, res, focalLen, sensSize, sensSize, fstop) {};

RealCamera::RealCamera(ui32_t width, ui32_t height, double focalLen, double sens_width,
    double sens_height, double fstop) : Camera(width, height), focalLength(focalLen), 
    fstop(fstop) {

    // Store the sensor dimensions
    sensorSize[0] = sens_width; 
    sensorSize[1] = sens_height;   

    for (int k = 0; k < 2; k++) {

        // Compute the Field of View (rad) and scale factor
        fov[k] = 2*atan(sensorSize[k]/(2*focalLength)); 
        scale[k] = tan(0.5*fov[k]); 

        // Compute the pixel physical size (mm/px)
        ui32_t res = k == 0 ? width : height;
        pixSize[k] = sensorSize[k]/res; 

    }

    // Compute aperture size (mm)
    aperture = focalLength/fstop;

}

Ray RealCamera::getRay(double u, double v, bool center) const {

    point3 origin;
    vec3 direction; 

    double x, y;

    if (center) {

        // We shoot a ray as if it were a Pinhole camera
        x = (2 * (u + 0.5)/(double)width() - 1) * scale[0];
        y = (2 * (v + 0.5)/(double)height() - 1) * scale[1];
        
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
        
        // TODO: Im not entirely certain this is correct...
        // For the direction, we sample the pixel in a random unit square around the center
        x = -0.5*(sensorSize[0] - pixSize[0]) + (u + randomNumber() - 0.5)*pixSize[0];
        y = -0.5*(sensorSize[1] - pixSize[1]) + (v + randomNumber() - 0.5)*pixSize[1];

        point3 pixSample(x, y, focalLength);

        // Compute the direction
        direction = pixSample - lensPoint;
    }

    return Ray(origin, _dcm*direction);

}

