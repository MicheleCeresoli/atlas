
#ifndef PIXEL_H
#define PIXEL_H

#include "vec2.h"
#include "vec3.h"


using Pixel = point2; 

struct PixelData {
    double t;       // Ray parameter at intersection 
    point3 s;       // Spherical coordinates of the intersection point
};

struct RenderedPixel {
    Pixel p; 
    PixelData d;
};


#endif 