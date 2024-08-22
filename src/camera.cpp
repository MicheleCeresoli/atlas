
#include "camera.h"
#include "color.h"

#include <cmath>
#include <iostream>

// Constructors 

Camera::Camera(int res_x, int res_y, double fov_x, double fov_y) :
    width(res_x), height(res_y), fov_x(fov_x), fov_y(fov_y), 
    dfov_x(fov_x/res_x), dfov_y(fov_y/res_y) {}

Camera::Camera(int res, double fov) : Camera(res, res, fov, fov) {} 


// Functions 

const dcm& Camera::get_dcm() const { return A; }
const point3& Camera::get_pos() const { return center; }

void Camera::set_dcm(const dcm& orientation) {
    A = orientation; 
}

void Camera::set_pos(const point3& pos) {
    center = pos; 
}

Ray Camera::get_ray(int u, int v) const {

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

void Camera::render(World& world)
{
    std::cout << "P3\n" << width << ' ' << height << "\n255\n"; 

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            Ray pixel_ray = get_ray(i, j);

            if (world.trace_ray(pixel_ray))
                write_color(std::cout, color(1,0,0)); 
            else
            {
                write_color(std::cout, color()); 
            }    
            
        }
    }

    std::clog << "\rDone. \n";

}

