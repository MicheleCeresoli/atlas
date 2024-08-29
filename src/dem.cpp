
#include "dem.h"
#include "utils.h"

DEM::DEM(std::vector<std::string> files, RenderingOptions opts) : 
    RasterContainer(files, opts.nThreads, opts.displayInfo) {

    // Initialise min\max altitude values
    _minAltitude = inf; 
    _maxAltitude = -inf;
    _meanRadius = 0.0;

    double hMin, hMax;
    RasterFile* pRaster;

    for (size_t k = 0; k < rasters.size(); k++)
    {
        pRaster = &rasters[k];

        // Retrieve raster band min\max altitude values
        hMin = pRaster->getRasterBand(0)->min(); 
        if (hMin < _minAltitude) {
            _minAltitude = hMin; 
        }

        hMax = pRaster->getRasterBand(0)->max(); 
        if (hMax > _maxAltitude) {
            _maxAltitude = hMax; 
        }
    }

    // Retrieve the DEM mean radius. This assumes that the mean value is equal in 
    // all rasters, which is true for the DEMs at hand. 
    if (nRasters() > 0)
        _meanRadius = rasters[0].crs()->GetSemiMajor(); 

}

DEM::DEM(std::string filename, RenderingOptions opts) : 
    DEM(std::vector<std::string>{filename}, opts) {}

