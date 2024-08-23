
#include "world.h"
#include "utils.h"

World::World() {}

PixelData World::trace_ray(Ray ray) 
{

    const double MAX_RADIUS = 1740e3; 
    // const double MIN_RADIUS = 1736e3; 
    const double RADIUS = 1737.8e3;

    const double dt = 20; 

    PixelData data; 
    data.t = inf;

    // The ray does not intersect the outer sphere
    if (ray.min_distance() > MAX_RADIUS) { 
        return data;
    }

    // Here we have an intersection. So we start by finding the two values 
    // of the t-parameter that define the search interval. 

    double tvals[2]; 
    ray.get_parameter(tvals, MAX_RADIUS); 

    double tk = tvals[0]; 
    point3 pos; 
    
    bool hit = false;
    while (!hit && tk <= tvals[1]) {
        
        // Compute ray position
        pos = ray.at(tk); 

        if (pos.norm() <= RADIUS) {
            hit = true;
            data.t = tk;  
            data.s = car2sph(pos); 
        } 

        tk += dt;

    }

    return data; 

}

