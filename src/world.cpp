
#include "world.h"
#include "utils.h"

#include <vector>

World::World(DEM &dem) : dem(dem) {

    // Initialise mean and minimum/maximum radius values
    meanRadius = dem.getMeanRadius(); 

    minRadius = dem.getMinAltitude() + meanRadius;
    maxRadius = dem.getMaxAltitude() + meanRadius; 

    // This will be updated on the `computeRayResolution` call. 
    dt = dem.getResolution(); 

}

PixelData World::trace_ray(Ray ray, int threadid) 
{

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

    double tk = MAX(0.0, tvals[0]); 
    double hk;

    point3 pos, sph; 
    point2 s2; 

    bool hit = false;
    while (!hit && tk <= tvals[1]) {
        
        // Compute ray position
        pos = ray.at(tk); 

        // Convert to spherical coordinates and retrieve longitude and latitude
        sph = car2sph(pos);

        // Convert geographic coordinates to degrees
        s2 = rad2deg(point2(sph[1], sph[2])); 

        // Retrieve altitude from DEM 
        hk = dem.getAltitude(s2, subsample, threadid); 

        if (sph[0] <= (hk + meanRadius)) {
             /* By putting t at halfway between the two values, we halve the maximum 
              * error we are commiting in the intersection location. */
            hit = true;
            data.t = tk - dt/2;
            data.s = car2sph(ray.at(data.t));   
        } 
        else if (sph[0] < minRadius) {

            /* This means that the ray has crossed the Moon in an area 
             * which does not have a loaded DEM available. Thus proceeding 
             * with any other computation does not make any sense. */
            break;
        }

        tk += dt;

    }

    return data; 

}

void World::computeRayResolution(const Camera& cam) {

    // Compute the minimum GSD of the camera 
    double gsd = computeGSD(cam); 

    // Retrieve the DEM's resolution 
    double resDem = dem.getResolution(); 

    // No point in using the maximum DEM's resolution
    if (gsd >= resDem) {

        /* Here we are reducing the resolution to avoid 'aliasing' issues. However, 
         * since we already satisfy the requirement, we don't want to slow down the 
         * computations too much, and thus we limit it at the maximum DEM's resolution. */

        dt = MAX(gsd/4, resDem); 
        dt = MIN(dt, 100.0);
        subsample = false;

    } else {
        /* Here we the DEM altitude is computed using a weighted-average on the 4 
         * neighbouring pixels to artificially improve the resolution of the DEM. */
        dt = gsd/2; 
        subsample = true;
    }

}

double World::computeGSD(const Camera& cam) const {

    // Compute the intersection points of each ray 
    std::vector<point3> fovPoints; 
    std::vector<bool>   tagPoints(cam.height, true);

    // Reserve enough space to avoid allocations.
    fovPoints.reserve(cam.height);

    double tks[2];
    double gsd = inf, gsd_jk; 

    for (size_t j = 0; j < cam.width; j++) {
        for (size_t k = 0; k < cam.height; k++) {
            
            // Retrieve ray
            Ray ray_k(cam.get_ray(j, k)); 

            if (ray_k.min_distance() > meanRadius) {
                tagPoints[k] = false; 
                continue;
            }

            // Compute position at moon intersection 
            ray_k.get_parameter(tks, meanRadius); 
            point3 uk(unit_vector(ray_k.at(tks[0])));

            // Check with pixel above
            if (k > 0) {
                if (tagPoints[k-1]) {
                    gsd_jk = meanRadius*acos(dot(uk, fovPoints[k-1]));
                    if (gsd_jk < gsd)
                        gsd = gsd_jk; 
                }
            } 
            
            // Check with pixel on the left
            if (j > 0) {
                if (tagPoints[k]) {
                    gsd_jk = meanRadius*acos(dot(uk, fovPoints[k])); 
                    if (gsd_jk < gsd) 
                        gsd = gsd_jk;
                }
            }

            // Store the unit vector pointing from the Moon center to the intersection point
            fovPoints[k] = uk;
            tagPoints[k] = true; 

        }
    }

    return gsd;

}