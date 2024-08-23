
#ifndef PIXEL_H
#define PIXEL_H

#include "vec3.h"

struct Pixel {
    int u; // Pixel Column (horizontal coordinate) 
    int v; // Pixel Row (vertical coordinate)
};

struct PixelData {
    double t;       // Ray parameter at intersection 
    point3 s;       // Spherical coordinates of the intersection point
};

struct RenderedPixel {
    Pixel p; 
    PixelData d;
};

#endif 