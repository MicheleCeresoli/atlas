
#include "world.h"
#include "utils.h"

World::World(RasterFile &dc) : dc(dc) {}

PixelData World::trace_ray(Ray ray, int threadid) 
{

    const double MAX_RADIUS = 1740e3; 
    // const double MIN_RADIUS = 1736e3; 
    const double RADIUS = 1737400;

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
    double hk;

    point3 pos, sph; 
    point2 s2, pix; 

    bool hit = false;
    while (!hit && tk <= tvals[1]) {
        
        // Compute ray position
        pos = ray.at(tk); 

        // Convert to spherical coordinates and retrieve longitude and latitude
        sph = car2sph(pos);
        s2.e[0] = sph[1]; 
        s2.e[1] = sph[2]; 

        // Compute map pixel coordiantes
        pix = dc.sph2pix(rad2deg(s2), threadid);

        // Retrieve altitude from DEM 
        // TODO: need to deal with the fact that data might not be available for that 
        // dataset. 
        hk = dc.getBandData(int(pix[0]), int(pix[1]));

        if (pos.norm() <= (hk + RADIUS)) {
            hit = true;
            data.t = tk;  
            data.s = car2sph(pos); 
        } 

        tk += dt;

    }

    return data; 

}

