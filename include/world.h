#ifndef WORLD_H 
#define WORLD_H 

#include "raster.h"
#include "ray.h"
#include "pixel.h"

class World {

    public: 

        World(RasterFile &dc);
        PixelData trace_ray(Ray r, int threaid);

        RasterFile dc; 

    private: 

};


#endif 