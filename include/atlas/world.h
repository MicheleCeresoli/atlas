#ifndef WORLD_H 
#define WORLD_H 

#include "camera.h"
#include "dem.h"
#include "dom.h"
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
            const Ray& r, double tMin, double tMax, int threadid, double maxErr = -1.0
    ); 
        
        inline double sampleDEM(const point2& p) { return dem.getData(p, interp, 0); }; 
        inline double sampleDOM(const point2& p) { return dom.getColor(p, interp, 0); };

        // DEM interface functions
        inline double maxRadius() const { return dem.maxRadius(); }
        inline double minRadius() const { return dem.minRadius(); }
        inline double meanRadius() const { return dem.meanRadius(); }
        
        inline double minAltitude() const { return dem.minAltitude(); }
        inline double maxAltitude() const { return dem.maxAltitude(); }

        // Compute the distance at which points are evaluated along a ray
        void computeRayResolution(const Camera* cam);
        // Manually update the ray resolution
        inline void setRayResolution(double res) { dt = res; }

        // Removes both DEM and DOM unused files
        void cleanup(); 
        // Unloads unused DEM files to reduce memory consumption.
        inline void cleanupDEM() { dem.cleanupRasters(opts.rasterUsageThreshold); }
        // Unloads unused DOM files to reduce memory consumption.
        inline void cleanupDOM() { dom.cleanupRasters(opts.rasterUsageThreshold); }

        inline double getRayResolution() const { return dt; }; 

        inline const std::vector<double>* getRayDistances() const { 
            return &rayDistances; 
        }

    private: 
    
        DEM dem;
        DOM dom;

        WorldOptions opts;
        
        double dt;  
        std::vector<double> rayDistances;
        
        // True if DEM data should be interpolated because of resolution limitations.
        bool interp = false;

        double computeGSD(const Camera* cam); 
        void findImpactLocation(
            PixelData& data, const Ray& ray, double tk, int threadid, double maxErr = -1.0
        );

};


#endif 
