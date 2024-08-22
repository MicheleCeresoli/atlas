
#ifndef UTILS_H 
#define UTILS_H 

#include <limits>

#define PI (3.14159265358979323846)

#define R2D (180.0/PI) 
#define D2R (PI/180.0)

const double inf = std::numeric_limits<double>::infinity();

double rad2deg(double x);
double deg2rad(double x); 

#endif 