#ifndef WORLD_H 
#define WORLD_H 

#include "ray.h"
#include "pixel.h"

class World {

    public: 

        World();
        PixelData trace_ray(Ray r);

};


#endif 