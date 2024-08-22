
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

double Ray::min_distance() const {
    // Compute the minimum distance of the ray from the origin
    return std::sqrt(p2 - pd2);
}

void Ray::get_parameter(double *t, double r) const {
    // Compute the parameter t at a given radial distance.
    double s = std::sqrt(pd2 - p2 + r*r); 
    t[0] = -pd - s; 
    t[1] = -pd + s; 
    return; 
}