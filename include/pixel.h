
#ifndef PIXEL_H
#define PIXEL_H

struct Pixel {
    int u; // Pixel Column (horizontal coordinate) 
    int v; // Pixel Row (vertical coordinate)
};

struct PixelData {
    double t;       // Ray parameter at intersection 
    double lat;     // Latitude (radians)
    double lon;     // Longitude (radians)
};

struct RenderedPixel {
    Pixel p; 
    PixelData d;
};

#endif 