#ifndef WORLD_H 
#define WORLD_H 

#include "camera.h"
#include "dem.h"
#include "dom.h"
#include "pixel.h"
#include "ray.h"
#include "settings.h"

#include <vector>
#include <string>


class World {

    public: 

        World(const WorldOptions& opts, uint nThreads);

        PixelData traceRay(const Ray& r, double tMin, double tMax, int threadid); 
        
        inline double sampleDEM(const point2& p) { return dem.getData(p, interp, 0); }; 
        inline double sampleDOM(const point2& p) { return dom.getColor(p, interp, 0); };

        // Compute the distance at which points are evaluated along a ray
        void computeRayResolution(const Camera* cam);
        // Manually update the ray resolution
        inline void setRayResolution(double res) { dt = res; }

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

};


#endif 
