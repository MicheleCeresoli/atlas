#ifndef WORLD_H 
#define WORLD_H 

#include "ray.h"

#define RADIUS 1.0

class World {

    public: 

        World();
        bool trace_ray(Ray r);

};


#endif 