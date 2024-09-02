
#include "pixel.h"
#include "utils.h"

#include <stdexcept>


/* -------------------------------------------------------
                        TASKED PIXEL
---------------------------------------------------------- */

TaskedPixel::TaskedPixel(uint id, double u, double v, size_t nSamples) : 
    id(id), nSamples(nSamples), tMin(0.0), tMax(inf) {

    this->u.reserve(nSamples);
    this->v.reserve(nSamples);

    // Fill the vectors with the desired number of samples
    for (size_t j = 0; j < nSamples; j++)  {
        this->u.push_back(u); 
        this->v.push_back(v); 
    }

}

void updateSSAACoordinates(TaskedPixel& tp) {

    double u = tp.u[0]; 
    double v = tp.v[0];

    if (tp.nSamples == 4) {
        
        tp.u[0] -= 0.25; 
        tp.u[1] += 0.25;
        tp.u[2] = tp.u[0]; 
        tp.u[3] = tp.u[1];
        tp.v[0] -= 0.25; 
        tp.v[1] = tp.v[0];
        tp.v[2] += 0.25; 
        tp.v[3] = tp.v[2];

    } else if (tp.nSamples == 8) {

        tp.u[0] -= 0.25; 
        tp.u[1] = tp.u[0];
        tp.u[2] = tp.u[0]; 
        tp.u[5] += 0.25;
        tp.u[6] = tp.u[5];
        tp.u[7] = tp.u[5];

        tp.v[0] -= 0.25; 
        tp.v[2] += 0.25; 
        tp.v[3] = tp.v[0];
        tp.v[4] = tp.v[2];
        tp.v[5] = tp.v[0];
        tp.v[7] = tp.v[2];

    } else if (tp.nSamples == 16) {

        v -= 0.375;
        u -= 0.375;

        size_t cnt = 0;
        for (size_t i = 0; i < tp.nSamples / 2; i++) {
            for (size_t j = 0; j < tp.nSamples / 2; j++) {

                tp.u[cnt] = u + 0.25*j;
                tp.v[cnt] = v + 0.25*i;
                
                cnt++;
            }
        }

    } else {
        std::invalid_argument("unsupported number of SSAA pixel samples.");
    }

}


/* -------------------------------------------------------
                        RENDERED PIXEL
---------------------------------------------------------- */

RenderedPixel::RenderedPixel(uint id, size_t nSamples) : 
    id(id), nSamples(nSamples), tMin(inf), tMax(-inf) {
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