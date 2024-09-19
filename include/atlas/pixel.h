
#ifndef PIXEL_H
#define PIXEL_H

#include "types.h"
#include "vec2.h"
#include "vec3.h"

#include <vector>

using Pixel = point2; 

#define MAX_PIX_SAMPLES     (8)

/**
 * @class TaskedPixel
 * @brief Class representing a pixel that is queued for a rendering task.
 */
class TaskedPixel {

    public: 

        /**
         * @brief Construct a new TaskedPixel object.
         * 
         * @param id Pixel ID.
         * @param u Pixel horizontal coordinate on the image plane.
         * @param v Pixel vertical coordinate on the image plane.
         * @param nSamples Number of samples for this pixel.
         */
        TaskedPixel(ui32_t id, double u, double v, size_t nSamples = 1);

        /**
         * @brief Pixel ID.
         */
        ui32_t id; 

        /**
         * @brief Number of samples taken for this pixel.
         */
        size_t nSamples;

        /**
         * @brief Minimum ray distance for the rendering task.
         */
        double tMin; 

        /**
         * @brief Maximum ray distance for the rendering task.
         */
        double tMax;

        /**
         * @brief Horizontal coordinates, on the image plane, of all the pixel samples.
         */
        std::vector<double> u; 

        /**
         * @brief Vertical coordinates, on the image plane, of all the pixel samples.
         */
        std::vector<double> v;
        
};

void updateSSAACoordinates(TaskedPixel& tp);


struct PixelData {

    double t;       // Ray parameter at intersection 
    point3 s;       // Spherical coordinates of the intersection point
};

class RenderedPixel {

    public: 

        ui32_t id;                        // Pixel id
        size_t nSamples;

        std::vector<PixelData> data;

        RenderedPixel(ui32_t id, size_t nSamples);
        inline RenderedPixel(TaskedPixel pix) : RenderedPixel(pix.id, pix.nSamples) {};

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