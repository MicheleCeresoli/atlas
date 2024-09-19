
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

/**
 * @class RenderedPixel
 * @brief Class storing all the rendered information for a given pixel.
 */
class RenderedPixel {

    public: 

        /**
         * @brief Pixel ID
         */
        ui32_t id;        
        
        /**
         * @brief Number of samples taken for this pixel.
         */
        size_t nSamples;

        /**
         * @brief Rendered pixel data for each sample.
         */
        std::vector<PixelData> data;

        /**
         * @brief Construct a new Rendered Pixel object.
         * 
         * @param id Rendered pixel ID.
         * @param nSamples Number of rendered samples for this pixel.
         */
        RenderedPixel(ui32_t id, size_t nSamples);

        /**
         * @brief Automatically construct a new RenderedPixel object from a TaskedPixel. 
         * @param pix Tasked pixel.
         * 
         * @note The new object automatically inherits the ID and the number of pixel 
         * samples from the input TaskedPixel.
         */
        inline RenderedPixel(TaskedPixel pix) : RenderedPixel(pix.id, pix.nSamples) {};

        /**
         * @brief Update the number of samples taken for this pixel.
         * @details This function only updates the total number of samples and reserves 
         * enough space within the internal vectors. 
         * @param newSamples Total number of rendered samples.
         */
        void updateSamples(size_t newSamples); 

        /**
         * @brief Add new rendered information. 
         * @param d Rendered pixel data.
         */
        void addPixelData(const PixelData& d); 

        // Return the distance of the first pixel sample (i.e., the ray from the center)

        /**
         * @brief Retun the distance of the first pixel sample (i.e., the ray from the 
         * center of the aperture to the center of the pixel.)
         * 
         * @return double Ray distance.
         */
        inline double pixDistance() const { return data[0].t; } 

        /**
         * @brief Return the minimum ray distance across all pixel samples.
         * @return double Minimum ray distance.
         */
        inline double pixMinDistance() const { return tMin; }; 

        /**
         * @brief Return the maximum ray distance across all pixel samples.
         * @return double Maximum ray distance.
         */
        inline double pixMaxDistance() const { return tMax; }; 

        /**
         * @brief Return the averaged distance across all pixel samples.
         * @return double Average distance.
         * 
         * @note Samples that have an infinite ray distance are excluded from the mean. 
         */
        double pixMeanDistance() const; 

    private: 

        double tMin; 
        double tMax; 
        

};



#endif 