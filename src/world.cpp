
#include "world.h"

World::World() {}

bool World::trace_ray(Ray r) {
    return r.min_distance() <= RADIUS;
}

