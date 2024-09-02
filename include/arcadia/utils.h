
#ifndef UTILS_H 
#define UTILS_H 


#include <cstdlib>
#include <limits>
#include <fstream>
#include <string>

#include "vec2.h"
#include "vec3.h"

#define PI (3.14159265358979323846)

#define R2D (180.0/PI) 
#define D2R (PI/180.0)

const double inf = std::numeric_limits<double>::infinity();

inline double rad2deg(double x) { return x*R2D; };
inline double deg2rad(double x) { return x*D2R; }; 

inline vec2 rad2deg(const vec2& v) { return vec2(v[0]*R2D, v[1]*R2D); }; 
inline vec2 deg2rad(const vec2& v) { return vec2(v[0]*D2R, v[1]*D2R); };

point3 car2sph(const point3& pos); 
point3 sph2car(const point3& sph); 

std::string readFileContent(const std::string& filename); 


// RANDOM NUMBER GENERATION

inline double randomNumber() { return std::rand() / (RAND_MAX + 1.0); }
inline double randomNumber(double min, double max) {
    return min + (max - min)*randomNumber();
}

#endif 