
#include "world.h"
#include "utils.h"

World::World(DEM &dem) : dem(dem) {

    // Initialise mean and maximum radius
    meanRadius = dem.getMeanRadius(); 
    maxRadius = dem.getMaxAltitude() + meanRadius; 
}

PixelData World::trace_ray(Ray ray, int threadid) 
{

    const double dt = 20; 

    PixelData data; 
    data.t = inf;

    // The ray does not intersect the outer sphere
    if (ray.min_distance() > maxRadius) { 
        return data;
    }

    // Here we have an intersection. So we start by finding the two values 
    // of the t-parameter that define the search interval. 

    double tvals[2]; 
    ray.get_parameter(tvals, maxRadius); 

    double tk = tvals[0] < 0.0 ? 0.0 : tvals[0]; 
    double hk;

    point3 pos, sph; 

    bool hit = false;
    while (!hit && tk <= tvals[1]) {
        
        // Compute ray position
        pos = ray.at(tk); 

        // Convert to spherical coordinates and retrieve longitude and latitude
        sph = car2sph(pos);

        // Retrieve altitude from DEM 
        hk = dem.getAltitude(rad2deg(sph[1]), rad2deg(sph[2]), threadid); 

        if (pos.norm() <= (hk + meanRadius)) {
            hit = true;
            data.t = tk;  
            data.s = car2sph(pos); 
        } 

        tk += dt;

    }

    return data; 

}

