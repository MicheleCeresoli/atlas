
#include "dem.h"

#include "utils.h"
#include "gdal_priv.h"

#include <cerrno>


DEM::DEM(std::vector<std::string> files, int nThreads) {

    // If there are no files, throw an error
    if (files.size() == 0)  {
        std::runtime_error("at least one DEM file is required");
    }

    // Initialise min\max altitude values
    _minAltitude = inf; 
    _maxAltitude = -inf;
    // Initialise the resolution.
    _resolution  = inf; 

    double hMin, hMax, res;

    // Load up all the rasters
    rasters.reserve(files.size()); 
    for (auto f : files)
    {
        rasters.push_back(RasterFile(f, nThreads));
        
        // Load the first band (its the only one for CE2 DEMs)
        rasters.back().loadBand(0); 

        // Retrieve raster band min\max altitude values
        hMin = rasters.back().getRasterBand(0)->min(); 
        if (hMin < _minAltitude) {
            _minAltitude = hMin; 
        }

        hMax = rasters.back().getRasterBand(0)->max(); 
        if (hMax > _maxAltitude) {
            _maxAltitude = hMax; 
        }

        // Update the minimum DEM resolution
        res = rasters.back().resolution();
        if (res < _resolution) {
            _resolution = res;
        }

    }    

    // Retrieve the DEM mean radius. This assumes that the mean value is equal in 
    // all rasters, which is true for the DEMs at hand. 
    _meanRadius = rasters[0].crs()->GetSemiMajor(); 

}

DEM::DEM(std::string filename, int nThreads) : 
    DEM(std::vector<std::string>{filename}, nThreads) {}

int DEM::nRasters() const { 
    return rasters.size();
}

double DEM::getMeanRadius() const { return _meanRadius; }
double DEM::getMinAltitude() const { return _minAltitude; }
double DEM::getMaxAltitude() const { return _maxAltitude; }

double DEM::getResolution() const { return _resolution; }

RasterFile DEM::getRasterFile(int i) const {
    return rasters[i];
}

double DEM::getAltitude(double lon, double lat, int threadid) const {

    point2 s(lon, lat); 
    point2 pix;  

    for (size_t k = 0; k < rasters.size(); k++) {
            
        if (rasters[k].isWithinGeographicBounds(lon, lat)) {
            pix = rasters[k].sph2pix(s, threadid); 
            return rasters[k].getBandData(pix[0], pix[1], 0); 
        }

    }

    return -inf; 

}
