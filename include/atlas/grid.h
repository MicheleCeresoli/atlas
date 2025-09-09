
#ifndef PIXELGRID_H 
#define PIXELGRID_H

#include "camera.h"
#include "types.h"

// TODO: add possibility to store the resolution 
// TODO: add capability to convert pixel ID to camera / cell 

class ScreenGrid {

    public: 

    ScreenGrid(ui32_t width, ui32_t height, const Camera* cam); 

    virtual ~ScreenGrid() = default; 

    inline ui32_t width() const { return _width; }
    inline ui32_t height() const { return _height; }

    inline ui32_t nPixels() const { return _width*_height; }

    ui32_t getPixelId(const ui32_t& u, const ui32_t& v); 
    
    private: 

        ui32_t _width = 0; 
        ui32_t _height = 0;

        // Coordinates of the top-left pixel 
        ui32_t u0, v0; 

        // Camera width and height
        ui32_t cam_width; 
        ui32_t cam_height;

};


#endif