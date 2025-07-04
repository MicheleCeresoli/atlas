
#include "world.h"
#include "utils.h"

#include <algorithm>


World::World(const WorldOptions& opts, ui32_t nThreads) : 
    dem(opts, nThreads), dom(opts, nThreads), opts(opts) {

    // This will be updated on the `computeRayResolution` call.
    dt = dem.getMaxResolution();

}

PixelData World::traceRay(
    const Ray& ray, double tMin, double tMax, ui32_t threadid, double maxErr
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
            findImpactLocation(data, ray, tk, threadid, maxErr);

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
    PixelData& data, const Ray& ray, double tk, ui32_t threadid, double maxErr
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

void World::computeRayResolution(const Camera* cam) {

    // Check whether at least one DEM raster is available, otherwise everything is pointless
    if (dem.nRasters() == 0) {
        throw std::runtime_error("No DEM rasters are available.");
    }

    /* Compute the minimum GSD of the camera and halve the resolution to avoid aliasing 
     * errors. */
    dt = 0.5*computeGSD(cam); 

    /* Ensure the resolution does not go below the allowed minimum. */
    dt = MAX(dt, opts.minRes);
    
    // Display the new ray resolution
    if (opts.logLevel >= LogLevel::DETAILED) {
        displayTime(); 

        if (std::isinf(dt)) {
            std::clog << "No valid ray intersection detected." << std::endl;
        } 
        else {
            std::clog << "Ray resolution set to: " 
                    << "\033[35m" << int(floor(dt)) << "m" << "\033[0m" << std::endl;
        }

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