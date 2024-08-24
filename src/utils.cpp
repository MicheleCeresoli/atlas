
#include "utils.h"
#include <cmath>

double rad2deg(double x) { return x*R2D; }
double deg2rad(double x) { return x*D2R; }

vec2 rad2deg(const vec2& v) {
    return vec2(v[0]*R2D, v[1]*R2D); 
}

vec2 deg2rad(const vec2& v) {
    return vec2(v[0]*D2R, v[1]*D2R);
}

// Convert cartesian pos to radius, longitude and latitude 
point3 car2sph(const point3& pos) {

    point3 s; 
    
    s[0] = pos.norm(); 
    s[1] = atan2(pos[1], pos[0]);
    s[2] = asin(pos[2]/s[0]);

    return s; 

}

// Convert radius, longitude and latitude to cartesian x,y,z pos
point3 sph2car(const point3& sph) {

    double clon = cos(sph[1]); 
    double slon = sin(sph[1]); 

    double rclat = sph[0]*cos(sph[2]); 
    double rslat = sph[0]*sin(sph[2]); 

    return point3(clon*rclat, slon*rclat, rslat);

}