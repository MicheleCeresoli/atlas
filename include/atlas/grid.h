
#ifndef PIXELGRID_H 
#define PIXELGRID_H

#include "camera.h"
#include "pixel.h"
#include "types.h"


class ScreenGrid {

    public: 


        ScreenGrid(Pixel p0, ui32_t width, ui32_t height, const Camera* cam); 

        virtual ~ScreenGrid() = default; 

        inline ui32_t width() const { return _width; }
        inline ui32_t height() const { return _height; }

        inline ui32_t nPixels() const { return _width*_height; }

        inline Pixel topLeft() const { return p0; }

        ui32_t getGPixelId(const ui32_t& ug, const ui32_t& vg) const; 
        ui32_t getGPixelId(const ui32_t& gid) const;

        inline ui32_t getGPixelId(const Pixel& pg) const { 
            return getGPixelId(pg[0], pg[1]); 
        }

        inline ui32_t getGPixelGridId(const ui32_t& ug, const ui32_t& vg) const {
            return ug + _width*vg;
        }

        inline ui32_t getGPixelGridId(const Pixel& pg) const { 
            return getGPixelGridId(pg[0], pg[1]); 
        }

        void getGPixelCoordinates(const ui32_t& gid, ui32_t& u, ui32_t& v) const; 
        void getGPixelGridCoordinates(const ui32_t& gid, ui32_t &ug, ui32_t& vg) const;

        inline void setRayResolution(double res) { dt = res; }
        inline double getRayResolution() const { return dt; }

        inline void addRayDistance(double distance) { rayDistances.push_back(distance); }
        inline const std::vector<double>* getRayDistances() const {
            return &rayDistances;
        }

        bool isRowAdaptiveRendering(); 

        
    private: 

        // Screen grid width and height
        ui32_t _width = 0; 
        ui32_t _height = 0;

        // Coordinates of the top-left pixel 
        Pixel p0;
        
        // ID of the top-left pixel
        ui32_t id0;

        // Camera width and height
        ui32_t cam_width; 
        ui32_t cam_height;

        // Ray resolutions
        double dt; 

        std::vector<double> rayDistances;
};

#endif