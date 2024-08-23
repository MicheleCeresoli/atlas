
#include "world.h"

World::World() {}

PixelData World::trace_ray(Ray r) {
    
    if (r.min_distance() <= RADIUS)
    {
        return PixelData {1.0, 0.0, 0.0};
    }
    else 
    {
        return PixelData {0.0, 0.0, 0.0};
    }

}

