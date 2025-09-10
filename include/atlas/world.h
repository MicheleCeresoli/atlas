#ifndef WORLD_H 
#define WORLD_H 

#include "camera.h"
#include "dem.h"
#include "dom.h"
#include "grid.h"
#include "pixel.h"
#include "ray.h"
#include "settings.h"
#include "types.h"

#include <vector>
#include <string>


class World {

    public: 

        World(const WorldOptions& opts, ui32_t nThreads);

        PixelData traceRay(
            const Ray& r, double dt, double tMin, double tMax, ui32_t threadid, 
            double maxErr = -1.0
    ); 
        
        inline double sampleDEM(const point2& p, double dt) { 
            return dem.getData(p, dt, 0);
        }; 

        inline double sampleDOM(const point2& p, double dt) { 
            return dom.getColor(p, dt, 0); 
        };

        // DEM interface functions
        inline double maxRadius() const { return dem.maxRadius(); }
        inline double minRadius() const { return dem.minRadius(); }
        inline double meanRadius() const { return dem.meanRadius(); }
        
        inline double minAltitude() const { return dem.minAltitude(); }
        inline double maxAltitude() const { return dem.maxAltitude(); }

        // Compute the distance at which points are evaluated along a ray
        void computeRayResolution(ScreenGrid& grid, const Camera* cam);

        // Removes both DEM and DOM unused files
        void cleanup(); 
        // Unloads unused DEM files to reduce memory consumption.
        inline void cleanupDEM() { dem.cleanupRasters(opts.rasterUsageThreshold); }
        // Unloads unused DOM files to reduce memory consumption.
        inline void cleanupDOM() { dom.cleanupRasters(opts.rasterUsageThreshold); }

    private: 
    
        DEM dem;
        DOM dom;

        WorldOptions opts;
        
        // True if DEM data should be interpolated because of resolution limitations.
        bool interp = false;

        double computeGSD(ScreenGrid& grid, const Camera* cam); 
        void findImpactLocation(
            PixelData& data, const Ray& ray, double dt, double tk, ui32_t threadid,
            double maxErr = -1.0
        );

};


#endif 
