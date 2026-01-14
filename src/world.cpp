
#include "world.h"
#include "utils.h"

#include <algorithm>


World::World(const WorldOptions& opts, ui32_t nThreads) : 
    dem(opts, nThreads), dom(opts, nThreads), opts(opts) {}

PixelData World::traceRay(
    const Ray& ray, double dt, double tMin, double tMax, ui32_t threadid, double maxErr
) 
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

        // Retrieve altitude from the DEM model.
        hk = dem.getData(s2, dt, threadid); 

        if (sph[0] <= (hk + dem.meanRadius())) {
            
            // We have an intersection
            hit = true;

            // Find the ray impact position minimising the localisation error.
            findImpactLocation(data, ray, dt, tk, threadid, maxErr);

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

void World::findImpactLocation(
    PixelData& data, const Ray& ray, double dt, double tk, ui32_t threadid, double maxErr
) {

    // We had an intersection at tk, thus we move backwards along the ray.
    double dtn = dt/2; 
    data.t = tk - dtn; 

    // The default error is equal to half the dem resolution
    if (maxErr <= 0.0) {
        maxErr = dem.getLastResolution(threadid);
    }
    
    point3 pos1 = ray.at(tk); 
    point3 pos2 = ray.at(data.t); 
    data.s = car2sph(pos2);
    
    /* If the position error is below the desired DEM resolution, we return straight away 
     * after halving the actual resolution, so that the maximum error we commit is 
     * still bounded to half the actual ray resolution. */

    point3 dp = pos2 - pos1;
    double errPos = dp.norm();
    if (errPos < maxErr) {
        return;
    }

    double hk; 
    point2 s2; 

    /* If we are here, we keep halving the ray resolution until the difference between 
     * the impact locations is below the desired dem resolution. */
    while (errPos > maxErr) {
        
        // Update ray resolution
        dtn /= 2;

        // Convert geographic coordinates to degrees
        s2 = rad2deg(point2(data.s[1], data.s[2])); 

        // Retrieve altitude from DEM 
        hk = dem.getData(s2, interp, threadid); 

        if (data.s[0] <= (hk + dem.meanRadius())) {
            // We have intersection, we need to move backwards
            data.t -= dtn;

        } else {
            // No intersection, we move forward
            data.t += dtn;
        }

        pos1 = pos2;
        pos2 = ray.at(data.t); 

        data.s = car2sph(pos2); 

        // Update position error
        dp = pos2 - pos1; 
        errPos = dp.norm(); 

    }
    
    return;

}

void World::computeRayResolution(ScreenGrid& grid, const Camera* cam) {

    // Check whether at least one DEM raster is available, otherwise everything is pointless
    if (dem.nRasters() == 0) {
        throw std::runtime_error("No DEM rasters are available.");
    }

    /* Compute the minimum GSD of the camera and halve the resolution to avoid aliasing 
     * errors. */
    double dt = 0.5*computeGSD(grid, cam); 

    /* Ensure the resolution does not go below or above the allowed boundaries. */
    dt = MAX(dt, opts.minRes);
    dt = MIN(dt, opts.maxRes);
    grid.setRayResolution(dt);

}

double World::computeGSD(ScreenGrid& grid, const Camera* cam) {

    // Compute the intersection points of each ray 
    std::vector<point3> fovPoints(grid.height()); 
    std::vector<bool>   tagPoints(grid.height(), true);

    double tMin, tMax;
    double gsd = inf, gsd_jk; 

    /* The GSD is usually computed using the average Moon radius for the reference sphere. 
     * This can cause issues when the camera position is below such distance. Thus, in 
     * this cases, the minimum sphere radius is used. */
    double sphereRadius = dem.meanRadius();

    // Check whether the camera is below the Moon average radius or not
    double posNorm = cam->getPos().norm();
    if (posNorm < dem.minRadius()) {
        return gsd;
    } else if (posNorm < dem.meanRadius()) {
        sphereRadius = dem.minRadius();
    }

    // Retrieve the coordinates of the top-left grid pixel.
    Pixel p0 = grid.topLeft();

    for (size_t j = 0; j < grid.width(); j++) {
        for (size_t k = 0; k < grid.height(); k++) {
            
            // Retrieve ray
            Ray ray_k(cam->getRay(j + p0[0], k + p0[1], true));

            // Compute position at moon intersection 
            ray_k.getParameters(sphereRadius, tMin, tMax); 

            /* If tMin is negative, the ray is "encountering" the Moon behind our back, 
             * so this case is treated as if we are missing it entirely. */

            if (ray_k.minDistance() > sphereRadius || tMin < 0) {
                tagPoints[k] = false;
                grid.addRayDistance(inf); 
                continue;
            }

            point3 uk(unit_vector(ray_k.at(tMin)));

            // Check with pixel above
            if (k > 0) {
                if (tagPoints[k-1]) {
                    gsd_jk = sphereRadius*acos(dot(uk, fovPoints[k-1]));
                    if (gsd_jk < gsd)
                        gsd = gsd_jk; 
                }
            } 
            
            // Check with pixel on the left
            if (j > 0) {
                if (tagPoints[k]) {
                    gsd_jk = sphereRadius*acos(dot(uk, fovPoints[k])); 
                    if (gsd_jk < gsd) 
                        gsd = gsd_jk;
                }
            }

            // Store the unit vector pointing from the Moon center to the intersection point
            fovPoints[k] = uk;
            tagPoints[k] = true; 

            grid.addRayDistance(tMin);

        } 

    }

    return gsd;

}

void World::cleanup() {
    // Unload both DEM and DOM unused files from memory
    cleanupDEM(); 
    cleanupDOM();
}