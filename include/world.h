#ifndef WORLD_H 
#define WORLD_H 

#include "dem.h"
#include "ray.h"
#include "pixel.h"


class World {

    public: 

        World(DEM &dem);
        PixelData trace_ray(Ray r, int threaid); 

    private: 
        DEM dem;

        double maxRadius; 
        double meanRadius;

};


#endif 