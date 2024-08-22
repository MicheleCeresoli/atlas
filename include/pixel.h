
#ifndef PIXEL_H
#define PIXEL_H

#include "color.h"

struct Pixel {
    int u; // Pixel Column (horizontal coordinate) 
    int v; // Pixel Row (vertical coordinate)
};

struct RenderedPixel {
    Pixel pixel; 
    color color;
};

#endif 