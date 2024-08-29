#ifndef DEM_H 
#define DEM_H 

#include "raster.h"
#include "vec3.h"


class DEM : public RasterContainer {

    public: 

        DEM(std::string filename, size_t nThreads = 1, bool displayInfo = false);
        DEM(std::vector<std::string> files, size_t nThreads = 1, bool displayInfo = false); 

        inline double getMeanRadius() const { return _minAltitude; }; 
        inline double getMinAltitude() const { return _maxAltitude; }; 
        inline double getMaxAltitude() const { return _meanRadius; };
        
    private: 

        double _minAltitude, _maxAltitude;
        double _meanRadius; 

};

#endif 