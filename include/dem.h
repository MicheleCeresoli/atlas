#ifndef DEM_H 
#define DEM_H 

#include "raster.h"
#include "vec3.h"


class DEM {

    public: 

        DEM(std::string filename, size_t nThreads = 1);
        DEM(std::vector<std::string> files, size_t nThreads = 1); 

        // Return the number of raster files
        size_t nRasters() const; 

        RasterFile getRasterFile(uint i) const; 

        double getMeanRadius() const; 
        double getMinAltitude() const; 
        double getMaxAltitude() const;

        // Return the highest resolution across all the loaded files
        double getResolution() const;  
        double getAltitude(const point2& s, bool interp, uint threadid = 0) const; 

    private: 

        std::vector<RasterFile> rasters;

        double _minAltitude, _maxAltitude;
        double _meanRadius; 

        double _resolution;

        double interpolateRaster(const point2& pix, size_t rid, int tid) const; 

};

#endif 