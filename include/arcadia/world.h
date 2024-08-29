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

        World(
            std::vector<std::string> dem_files, 
            std::vector<std::string> dom_files,
            RenderingOptions opts
        );

        PixelData trace_ray(const Ray& r, const double* tvals, int threadid); 

        // Compute the distance at which points are evaluated along a ray
        void computeRayResolution(const Camera& cam);

        // Unloads unused DEM files to reduce memory consumption.
        inline void cleanupDEM() { dem.cleanupRasters(); }
        // Unloads unused DOM files to reduce memory consumption.
        inline void cleanupDOM() { dom.cleanupRasters(); }

        inline double getRayResolution() const { return dt; }; 

    private: 
    
        DEM dem;
        DOM dom;

        double minRadius;
        double maxRadius; 
        double meanRadius;
        
        double dt;  
        
        // True if DEM data should be interpolated because of resolution limitations.
        bool interp = false;

        double computeGSD(const Camera& cam) const; 

};


#endif 