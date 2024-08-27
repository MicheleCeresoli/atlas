#ifndef DEM_H 
#define DEM_H 

#include "raster.h"
#include "vec3.h"


class DEM {

    public: 

        DEM(std::string filename, int nThreads = 1);
        DEM(std::vector<std::string> files, int nThreads = 1); 

        // Return the number of raster files
        int nRasters() const; 

        RasterFile getRasterFile(int i) const; 

        double getMeanRadius() const; 
        double getMinAltitude() const; 
        double getMaxAltitude() const;

        // Return the highest resolution across all the loaded files
        double getResolution() const;  

        double getAltitude(double lon, double lat, int threadid = 0) const; 

    private: 

        std::vector<RasterFile> rasters;

        double _minAltitude, _maxAltitude;
        double _meanRadius; 

        double _resolution;

};

#endif 