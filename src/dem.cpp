
#include "dem.h"
#include "utils.h"

DEM::DEM(const std::vector<std::string>& files, uint nThreads, bool displayLogs) : 
    RasterContainer(files, nThreads, displayLogs) {

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

    _minRadius = _meanRadius + _minAltitude; 
    _maxRadius = _meanRadius + _maxAltitude;

}

DEM::DEM(WorldOptions opts, uint nThreads) : 
    DEM(opts.demFiles, nThreads, opts.logLevel >= LogLevel::MINIMAL) {}