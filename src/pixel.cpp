
#include "pixel.h"
#include "utils.h"

#include <stdexcept>


/* -------------------------------------------------------
                        TASKED PIXEL
---------------------------------------------------------- */

TaskedPixel::TaskedPixel(uint id, double i, double j, size_t nSamples) : 
    id(id), nSamples(nSamples) 
{

    if (nSamples == 1) {
        u[0] = i;
        v[0] = j;

    } else if (nSamples == 4) {

        u[0] = i - 0.25; 
        u[1] = i + 0.25;
        u[2] = u[0]; 
        u[3] = u[1];

        v[0] = j - 0.25; 
        v[1] = v[0];
        v[2] = j + 0.25; 
        v[3] = v[2];

    } else if (nSamples = 8) {

        u[0] = i - 0.25; 
        u[1] = u[0];
        u[2] = u[0]; 
        u[3] = i;
        u[4] = i;
        u[5] = i + 0.25;
        u[6] = u[5];
        u[7] = u[5];

        v[0] = j - 0.25; 
        v[1] = j;
        v[2] = j + 0.25; 
        v[3] = v[0];
        v[4] = v[2];
        v[5] = v[0];
        v[6] = v[1];
        v[7] = v[2];

    } else {
        std::invalid_argument("unsupported number of pixel samples.");
    }

}

TaskedPixel::TaskedPixel(uint id, point2 p, size_t nSamples) : 
    TaskedPixel(id, p[0], p[1], nSamples) {}




/* -------------------------------------------------------
                        RENDERED PIXEL
---------------------------------------------------------- */

RenderedPixel::RenderedPixel(uint id, size_t nSamples) : 
    id(id), nSamples(nSamples), tMin(inf), tMax(-inf) 
{
    data.reserve(nSamples); 

}

void RenderedPixel::updateSamples(size_t newSamples) {
    nSamples = newSamples;
    data.reserve(nSamples);
}

void RenderedPixel::addPixelData(const PixelData& d) {
    
    // Add the data
    data.push_back(d); 

    // Update distance limits 

    if (d.t < tMin)
        tMin = d.t; 
    
    if (d.t > tMax)
        tMax = d.t; 

}


double RenderedPixel::pixMeanDistance() const {
    double t; 

    for (size_t j = 0; j < nSamples; j++) {
        if (data[j].t != inf)
            t += data[j].t; 
    }

    t /= nSamples;
    return t;

}