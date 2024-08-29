
#include "world.h"
#include "utils.h"

World::World(
    std::vector<std::string> dem_files, 
    std::vector<std::string> dom_files, 
    RenderingOptions opts
) : dem(dem_files, opts), dom(dom_files, opts) {

    // Initialise mean and minimum/maximum radius values
    meanRadius = dem.getMeanRadius(); 

    minRadius = dem.getMinAltitude() + meanRadius;
    maxRadius = dem.getMaxAltitude() + meanRadius; 

    // This will be updated on the `computeRayResolution` call. 
    dt = dem.getResolution(); 

}

PixelData World::trace_ray(const Ray& ray, const double* tint, int threadid) 
{

    PixelData data; 
    data.t = inf;

    // The ray does not intersect the outer sphere
    if (ray.min_distance() > maxRadius) { 
        return data;
    }

    /* Here we have an intersection. So we start by finding the two values
     * of the t-parameter that define the search interval. Then, depending on whether 
     * an interval bound was provided to the tracer, the t-boundaries are 
     * properly adjusted. */

    double tk, tvals[2];
    ray.get_parameter(tvals, maxRadius); 

    if (tint[0] == 0.0 && tint[1] == 0.0) {
        tk = tvals[0]; 
    } 
    else {
        tk = tint[0] - 3*dt;
        tvals[1] = MIN(tvals[1], tint[1] + 3*dt);  
    }

    // Starting t-value
    tk = MAX(0.0, tk); 

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
        hk = dem.getData(s2, interp, threadid); 

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