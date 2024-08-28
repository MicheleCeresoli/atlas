
#ifndef UTILS_H 
#define UTILS_H 

#include <limits>
#include <fstream>
#include <string>

#include "vec2.h"
#include "vec3.h"

#define PI (3.14159265358979323846)

#define R2D (180.0/PI) 
#define D2R (PI/180.0)

const double inf = std::numeric_limits<double>::infinity();

double rad2deg(double x);
double deg2rad(double x); 

vec2 rad2deg(const vec2& v); 
vec2 deg2rad(const vec2& v); 

point3 car2sph(const point3& pos); 
point3 sph2car(const point3& sph); 

std::string readFileContent(const std::string& filename); 

#endif 