
#include "utils.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>


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

void displayTime() {

    // Get the current time point
    auto now = std::chrono::system_clock::now();
    
    // Convert to time_t to use with std::localtime
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    // Convert to tm structure for local time
    std::tm* local_time = std::localtime(&current_time);

    std::clog << "\r\033[33m[" << std::put_time(local_time, "%H:%M:%S") << "]\033[0m ";

}