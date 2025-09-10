
 #include "grid.h"
 #include "utils.h"

 ScreenGrid::ScreenGrid(
    Pixel p0, ui32_t width, ui32_t height, const Camera* cam
) : p0(p0), _width(width), _height(height) {

    // Retrieve the camera height and width resolutions
    cam_height = cam->height();
    cam_width = cam->width();

    // Pre-compute the ID of the top pixel
    id0 = cam->getPixelId(p0);

    // Initialize ray distance vector 
    rayDistances.reserve(width*height);

};

ui32_t ScreenGrid::getGPixelId(const ui32_t& ug, const ui32_t& vg) const {

    // Retrieve the coordinates of the top-left pixel
    ui32_t u0 = p0[0]; 
    ui32_t v0 = p0[1];

    // Compute the coordinates with respect to the global camera
    ui32_t u = u0 + ug; 
    ui32_t v = v0 + vg; 
    
    return u + v*cam_width;

}

ui32_t ScreenGrid::getGPixelId(const ui32_t& gid) const {
    
    // Recover the pixel grid coordinates
    ui32_t ug, vg; 
    getGPixelGridCoordinates(gid, ug, vg);

    // Transform these coordinates into a camera ID 
    return getGPixelId(ug, vg);
}

void ScreenGrid::getGPixelCoordinates(const ui32_t& gid, ui32_t& u, ui32_t& v) const {

    /* Here we retrieve the pixel coordinates with respect to the camera, starting from 
     * the original pixel grid id. */

    // Compute the pixel coordinates with respect to the grid
    ui32_t ug, vg; 
    getGPixelGridCoordinates(gid, ug, vg); 

    // Add the offset caused by the top-left pixel.
    u = ug + p0[0]; 
    v = vg + p0[1];

}

void ScreenGrid::getGPixelGridCoordinates(const ui32_t& gid, ui32_t& ug, ui32_t& vg) const {

    /* TODO: this could overflow because we are subtracting two numbers that are always 
     * meant to be positive. */
    vg = gid / _width; 
    ug = gid - vg*_width;

}

bool ScreenGrid::isRowAdaptiveRendering() {

    /* The idea is the following: we need to find out whether the variation along 
     * the ray distances is greater along the rows or the columns. This allows to avoid 
     * issues when all the pixels are at more or less the same distance causing the 
     * actual closest pixel to be further from its neighbours when the DEM is used. */

    double cMeanDist = 0.0; 
    double rMeanDist = 0.0; 

    double dMin, dMax, dj; 

    size_t u, v; 

    // Compute the average distance variation along the columns 
    for (u = 0; u < _width; u++) {

        dMin =  inf; 
        dMax = -inf;

        for (v = 0; v < _height; v++) {
            dj = rayDistances.at(v + u*_height);

            // Update the min/max distance 
            if (dj != inf) {
                dMin = dj < dMin ? dj : dMin; 
                dMax = dj > dMax ? dj : dMax;
            }
        }

        if (dMax > 0.0) { 
            cMeanDist += dMax - dMin;
        }
    }

    // Compute the average distance variation along the rows
    for (v = 0; v < _height; v++) {

        dMin =  inf; 
        dMax = -inf;

        for (u = 0; u < _width; u++) {
            dj = rayDistances.at(v + u*_height); 

            // Update the min/max distance 
            if (dj != inf) {
                dMin = dj < dMin ? dj : dMin; 
                dMax = dj > dMax ? dj : dMax;
            }

        }

        if (dMax > 0.0) {
            rMeanDist += dMax - dMin;
        }
    }

    // Average the distance between the number of columns/rows 
    cMeanDist /= _width; 
    rMeanDist /= _height; 

    // Row adaptive when the distance excursion along the rows is greater than cols
    return rMeanDist > cMeanDist;

}

