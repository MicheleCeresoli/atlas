#ifndef WORLD_H 
#define WORLD_H 

#include "camera.h"
#include "dem.h"
#include "ray.h"
#include "pixel.h"


class World {

    public: 

        World(DEM &dem);
        PixelData trace_ray(const Ray& r, const double* tvals, int threadid); 

        // Compute the distance at which points are evaluated along a ray
        void computeRayResolution(const Camera& cam);

        inline double getRayResolution() const { return dt; }; 

    private: 
    
        DEM dem;

        double minRadius;
        double maxRadius; 
        double meanRadius;
        
        double dt;  
        
        // True if DEM data should be interpolated because of resolution limitations.
        bool interp = false;

        double computeGSD(const Camera& cam) const; 

};


#endif 