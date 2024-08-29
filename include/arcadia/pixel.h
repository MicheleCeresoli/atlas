
#ifndef PIXEL_H
#define PIXEL_H

#include "vec2.h"
#include "vec3.h"

#include <vector>

using Pixel = point2; 

#define MAX_PIX_SAMPLES     (8)

class TaskedPixel {

    public: 
    
        TaskedPixel(uint id, double i, double j, size_t nSamples = 1);
        TaskedPixel(uint id, point2 p, size_t nSamples = 1); 

        uint id; 
        size_t nSamples = 1;

        double u[MAX_PIX_SAMPLES]; 
        double v[MAX_PIX_SAMPLES]; 

        double tint[2] = {0.0, 0.0}; 
};

struct PixelData {

    double t;       // Ray parameter at intersection 
    point3 s;       // Spherical coordinates of the intersection point
};

class RenderedPixel {

    public: 

        uint id;                        // Pixel id
        size_t nSamples;

        std::vector<PixelData> data;

        RenderedPixel(uint id, size_t nSamples);

        void updateSamples(size_t newSamples); 
        void addPixelData(const PixelData& d); 

        // Return the distance of the first pixel sample (i.e., the ray from the center)
        inline double pixDistance() const { return data[0].t; } 

        inline double pixMinDistance() const { return tMin; }; 
        inline double pixMaxDistance() const { return tMax; }; 

        // Return the averaged distance across all pixel samples
        double pixMeanDistance() const; 

    private: 

        double tMin; 
        double tMax; 
        

};



#endif 