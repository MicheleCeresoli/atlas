#ifndef DEM_H 
#define DEM_H 

#include "raster.h"
#include "settings.h"

class DEM : public RasterContainer {

    public: 


        DEM(WorldOptions opts, uint nThreads);
        DEM(const std::vector<std::string>& files, uint nThreads, bool displayLogs); 
        
        inline double minAltitude() const { return _minAltitude; }; 
        inline double maxAltitude() const { return _maxAltitude; };

        inline double minRadius() const { return _minRadius; }
        inline double maxRadius() const { return _maxRadius; }

        inline double meanRadius() const { return _meanRadius; }; 

        
    private: 

        double _minAltitude, _maxAltitude;
        double _minRadius, _maxRadius, _meanRadius;
};

#endif 