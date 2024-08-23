#ifndef WORLD_H 
#define WORLD_H 

#include "ray.h"
#include "pixel.h"

#define RADIUS 1.0

class World {

    public: 

        World();
        PixelData trace_ray(Ray r);

};


#endif 