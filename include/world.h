#ifndef WORLD_H 
#define WORLD_H 

#include "camera.h"
#include "dem.h"
#include "ray.h"
#include "pixel.h"


class World {

    public: 

        World(DEM &dem);
        PixelData trace_ray(Ray r, int threaid); 

        // Compute the distance at which points are evaluated along a ray
        void computeRayResolution(const Camera& cam);

    private: 
    
        DEM dem;

        double minRadius;
        double maxRadius; 
        double meanRadius;
        
        double dt;  
        bool subsample = false;

        double computeGSD(const Camera& cam) const; 

};


#endif 