
#include "dem.h"

#include "utils.h"
#include "gdal_priv.h"

#include <cerrno>
#include <chrono>
#include <iomanip>

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
    int progress; 

    // Store current time
    auto t1 = std::chrono::high_resolution_clock::now();

    // Load up all the rasters
    rasters.reserve(files.size()); 
    for (auto f : files)
    {
        // Add the raster and retrieve its name.
        rasters.push_back(RasterFile(f, nThreads));
        RasterFile* pRaster = &rasters.back();

        // Update loading status
        progress = (int)(100*(double)rasters.size()/files.size());

        std::clog << "\r[" <<  std::setw(3) << progress 
                  << "%] \033[32mLoading DEM file:\033[0m " 
                  << pRaster->getFileName() << std::flush;
        
        // Load the first band (its the only one for CE2 DEMs)
        pRaster->loadBand(0); 

        // Retrieve raster band min\max altitude values
        hMin = pRaster->getRasterBand(0)->min(); 
        if (hMin < _minAltitude) {
            _minAltitude = hMin; 
        }

        hMax = pRaster->getRasterBand(0)->max(); 
        if (hMax > _maxAltitude) {
            _maxAltitude = hMax; 
        }

        // Update the minimum DEM resolution
        res = pRaster->resolution();
        if (res < _resolution) {
            _resolution = res;
        }

    }    

    // Retrieve time to compute rendering duration
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);

    std::clog << std::endl << "       DEM files loaded in " << duration.count() 
              << " seconds." << std::endl;

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
