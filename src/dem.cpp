
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

double DEM::getAltitude(const point2& s, bool subsample, int threadid) const {

    point2 pix;  

    for (size_t k = 0; k < rasters.size(); k++) {
            
        if (rasters[k].isWithinGeographicBounds(s)) {
            pix = rasters[k].sph2pix(s, threadid); 

            if (!subsample) {
                return rasters[k].getBandData((int)pix[0], (int)pix[1], 0); 
            } else {
                return subsampleRaster(pix, k, threadid);
            }
        }

    }

    return -inf; 

}

double DEM::subsampleRaster(const point2& pix, int rid, int tid) const {

    int u = (int)pix[0], v = (int)pix[1]; 

    // These are the upper-left (dr) and bottom-right (dl) points
    point2 dl(u, v); 
    point2 dr = dl + 1;

    dl = pix - dl; 
    dr = dr - pix; 
    
    // Retrieve no data value
    double noDataVal = rasters[rid].getBandNoDataValue(0); 

    // Retrieve raster width and height 
    double s = sqrt(2), sk; 
    double h, n = 0.0, d = 0.0; 

    bool hu = (u+1) < rasters[rid].width(); 
    bool hv = (v+1) < rasters[rid].height(); 

    for (size_t j = 0; j < 4; j++) {

        switch (j) {
            case 0:
                sk = dl.norm();
                h = rasters[rid].getBandData(u, v, 0); 
                break;

            case 1: 
                sk = sqrt(dr[0]*dr[0] + dl[1]*dl[1]);
                h = hu ? rasters[rid].getBandData(u+1, v, 0) : noDataVal; 
                break;

            case 2: 
                sk = sqrt(dl[0]*dl[0] + dr[1]*dr[1]); 
                h = hv ? rasters[rid].getBandData(u, v+1, 0) : noDataVal; 
                break;

            default: 
                sk = dr.norm(); 
                h = (hu && hv) ? rasters[rid].getBandData(u+1, v+1, 0) : noDataVal; 
                break;
        }

        if (h != noDataVal) {
            sk = 1 - sk/s; 
            n += sk*h;
            d += sk; 
        } 

    }

    return n/d; 

}
