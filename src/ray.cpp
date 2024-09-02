
#include "ray.h"
#include <cmath> 

Ray::Ray(const point3& origin, const vec3& direction) : 
    p(origin), d(unit_vector(direction)), pd(dot(origin, d)), 
    pd2(pd*pd), p2(origin.norm2()) {}

const point3& Ray::origin() const { return p; }
const vec3& Ray::direction() const { return d; }

point3 Ray::at(double t) const {
    // Evaluate the position of the ray at a given t-value.
    return p + t*d; 
}

double Ray::minDistance() const {
    // Compute the minimum distance of the ray from the origin
    return std::sqrt(p2 - pd2);
}

void Ray::getParameters(double r, double& tMin, double& tMax) const {
    
    // Compute the parameter t at a given radial distance.
    double s = std::sqrt(pd2 - p2 + r*r);

    // Compute minimum and maximum values 
    tMin = -pd - s; 
    tMax = -pd + s;  
    
}