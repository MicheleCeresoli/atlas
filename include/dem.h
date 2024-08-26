
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

        double getAltitude(point3 pos) const; 

    private: 

        std::vector<RasterFile> rasters;

        double _minAltitude, _maxAltitude;
        double _meanRadius; 

};