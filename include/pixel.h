
#ifndef PIXEL_H
#define PIXEL_H

#include "vec2.h"
#include "vec3.h"

#include <vector>

using Pixel = point2; 

#define MAX_PIX_SAMPLES     (8)

struct TaskedPixel {

    uint id; 
    size_t nSamples = 1;

    double u[MAX_PIX_SAMPLES]; 
    double v[MAX_PIX_SAMPLES]; 

    double tint[2] = {0.0, 0.0}; 
    
    TaskedPixel(uint id, double i, double j, size_t nSamples = 1);
    TaskedPixel(uint id, point2 p, size_t nSamples = 1); 
};

struct PixelData {
    double t;       // Ray parameter at intersection 
    point3 s;       // Spherical coordinates of the intersection point
};

struct RenderedPixel {

    uint id;                        // Pixel id
    size_t nSamples;

    std::vector<PixelData> data;

    RenderedPixel(uint id, size_t nSamples);

    void updateSamples(size_t newSamples); 
    void addPixelData(const PixelData& d); 

};



#endif 