#ifndef WORLD_H 
#define WORLD_H 

#include "ray.h"
#include "color.h"

#define RADIUS 1.0

class World {

    public: 

        World();
        Color trace_ray(Ray r);

};


#endif 