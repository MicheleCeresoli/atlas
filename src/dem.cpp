
#include "dem.h"
#include "utils.h"

DEM::DEM(const std::vector<RasterDescriptor>& descriptors, ui32_t nThreads, bool displayLogs) : 
    RasterManager(descriptors, nThreads, displayLogs) {

    // Initialise min\max altitude values
    _minAltitude = inf; 
    _maxAltitude = -inf;
    _meanRadius = inf;

    double hMin, hMax, meanRadius;
    const RasterFile* pRaster;

    // Iterate among the raster containers
    for (size_t k = 0; k < containers.size(); k++)
    {
        // Iterate among the rasters in the container 
        for (size_t j = 0; j < containers[k]->nRasters(); j++) {

            pRaster = containers[k]->getRasterFile(j);

            // Retrieve raster band min\max altitude values
            hMin = pRaster->getRasterBand(0)->min(); 
            if (hMin < _minAltitude) {
                _minAltitude = hMin; 
            }

            hMax = pRaster->getRasterBand(0)->max(); 
            if (hMax > _maxAltitude) {
                _maxAltitude = hMax; 
            }

            // Retrieve the DEM mean radius and ensure its the same across all rasters.
            meanRadius = pRaster->crs()->GetSemiMajor(); 
            if (std::isinf(_meanRadius)) {
                _meanRadius = meanRadius;
            } else if (std::fabs(_meanRadius - meanRadius) > 1e-9) {
                throw std::runtime_error(
                    "All raster files must have the same reference body radius."
                );
            }

        }
    }

    _minRadius = _meanRadius + _minAltitude; 
    _maxRadius = _meanRadius + _maxAltitude;

}

DEM::DEM(WorldOptions opts, ui32_t nThreads) : 
    DEM(opts.demFiles, nThreads, opts.logLevel >= LogLevel::MINIMAL) {}