
#include "world.h"

World::World() {}

Color World::trace_ray(Ray r) {
    
    if (r.min_distance() <= RADIUS)
    {
        return Color(1, 0, 0);
    }
    else 
    {
        return Color(); 
    }

}

