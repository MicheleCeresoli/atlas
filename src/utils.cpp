
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


std::string readFileContent(const std::string& filename) {
    
    // Open the file
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file) {
        throw std::runtime_error("failed to open the file");
    }
    
    // Seek to the end of the file to get its size
    file.seekg(0, std::ios::end);
    std::size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a string of the appropriate size
    std::string content(fileSize, '\0');

    // Read the entire file into the string
    file.read(&content[0], fileSize);

    return content; 
}
