
#include "world.h"
#include "utils.h"

#include <algorithm>


World::World(const WorldOptions& opts, uint nThreads) : 
    dem(opts, nThreads), dom(opts, nThreads), opts(opts) {

    // This will be updated on the `computeRayResolution` call. 
    dt = dem.getResolution(); 

}

PixelData World::traceRay(const Ray& ray, double tMin, double tMax, int threadid) 
{

    PixelData data; 
    data.t = inf;

    // The ray does not intersect the outer sphere
    if (ray.minDistance() > dem.maxRadius()) { 
        return data;
    }

    /* Here we have an intersection. So we start by finding the two values
     * of the t-parameter that define the search interval. Then, depending on whether 
     * an interval bound was provided to the tracer, the t-boundaries are 
     * properly adjusted. */

    double tk, tEnd;
    ray.getParameters(dem.maxRadius(), tk, tEnd); 

    /* If tk is negative, it means that the ray encounters the Moon in a direction 
     * opposite to the one the camera is facing. However, if the camera position is 
     * above the maximum altitude of the Moon, there's now way one a ray like that 
     * can intersect a mountain. */

    if ((tk < 0.0) && (ray.origin().norm() > dem.maxRadius())) {
        return data; 
    }

    if (tMin != 0.0) {
        tk = tMin;
    } 

    if (tMax != 0.0) {
        tEnd = tEnd < tMax ? tEnd : tMax;
    }

    // Starting t-value can't be smaller than 0.0 (not going backwards!)
    tk = tk > 0 ? tk : 0.0; 

    double hk; 

    point3 pos, sph; 
    point2 s2; 

    bool hit = false;
    while (!hit && tk <= tEnd) {

        // Compute ray position
        pos = ray.at(tk); 

        // Convert to spherical coordinates and retrieve longitude and latitude
        sph = car2sph(pos);

        // Convert geographic coordinates to degrees
        s2 = rad2deg(point2(sph[1], sph[2])); 

        // Retrieve altitude from DEM 
        hk = dem.getData(s2, interp, threadid); 

        if (sph[0] <= (hk + dem.meanRadius())) {
             /* By putting t at halfway between the two values, we halve the maximum 
              * error we are commiting in the intersection location. */
            hit = true;
            data.t = tk - dt/2;
            data.s = car2sph(ray.at(data.t));   
        } 
        else if (sph[0] < dem.minRadius()) {
            /* This means that the ray has crossed the Moon in an area 
             * which does not have a loaded DEM available. Thus proceeding 
             * with any other computation does not make any sense. */
            break;
        }

        tk += dt;
    }

    return data; 

}

void World::computeRayResolution(const Camera* cam) {

    // Compute the minimum GSD of the camera 
    double gsd = computeGSD(cam); 

    // Retrieve the DEM's resolution 
    double resDem = dem.getResolution(); 

    interp = gsd < resDem; 
    
    // No point in using the maximum DEM's resolution
    if (interp) {

        /* Here we are reducing the resolution to avoid 'aliasing' issues. However, 
         * since we already satisfy the requirement, we don't want to slow down the 
         * computations too much, and thus we limit it at the maximum DEM's resolution. */

        dt = MAX(gsd/4, resDem); 
        dt = MIN(dt, 100.0);

    } else {
        /* Here we the DEM altitude is computed using a weighted-average on the 4 
         * neighbouring pixels to artificially improve the resolution of the DEM. */
        dt = gsd/2; 
    }

    // Display the new ray resolution
    if (opts.logLevel >= LogLevel::DETAILED) {
        displayTime(); 
        std::clog << "Ray resolution set to: " 
                  << "\033[35m" << int(floor(dt)) << "m" << "\033[0m" << std::endl;
    }

}

double World::computeGSD(const Camera* cam) {

    // Update the array with the distances of each pixel
    rayDistances.clear(); 
    rayDistances.reserve(cam->nPixels()); 

    // Compute the intersection points of each ray 
    std::vector<point3> fovPoints(cam->height()); 
    std::vector<bool>   tagPoints(cam->height(), true);

    double tMin, tMax;
    double gsd = inf, gsd_jk; 

    for (size_t j = 0; j < cam->width(); j++) {
        for (size_t k = 0; k < cam->height(); k++) {
            
            // Retrieve ray
            Ray ray_k(cam->getRay(j, k, true)); 

            // Compute position at moon intersection 
            ray_k.getParameters(dem.meanRadius(), tMin, tMax); 

            /* If tMin is negative, the ray is "encountering" the Moon behind our back, 
             * so this case is treated as if we are missing it entirely. */

            if (ray_k.minDistance() > dem.meanRadius() || tMin < 0) {
                tagPoints[k] = false;
                rayDistances.push_back(inf); 
                continue;
            }

            point3 uk(unit_vector(ray_k.at(tMin)));

            // Check with pixel above
            if (k > 0) {
                if (tagPoints[k-1]) {
                    gsd_jk = dem.meanRadius()*acos(dot(uk, fovPoints[k-1]));
                    if (gsd_jk < gsd)
                        gsd = gsd_jk; 
                }
            } 
            
            // Check with pixel on the left
            if (j > 0) {
                if (tagPoints[k]) {
                    gsd_jk = dem.meanRadius()*acos(dot(uk, fovPoints[k])); 
                    if (gsd_jk < gsd) 
                        gsd = gsd_jk;
                }
            }

            // Store the unit vector pointing from the Moon center to the intersection point
            fovPoints[k] = uk;
            tagPoints[k] = true; 

            rayDistances.push_back(tMin);

        } 

    }

    return gsd;

}

void World::cleanup() {
    // Unload both DEM and DOM unused files from memory
    cleanupDEM(); 
    cleanupDOM();
}