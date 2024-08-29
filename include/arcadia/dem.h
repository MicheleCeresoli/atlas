#ifndef DEM_H 
#define DEM_H 

#include "raster.h"
#include "settings.h"
#include "vec3.h"


class DEM : public RasterContainer {

    public: 


        DEM(std::string filename, RenderingOptions opts);
        DEM(std::vector<std::string> files, RenderingOptions opts); 

        DEM(const DEM&) = default; 
        
        inline double getMeanRadius() const { return _meanRadius; }; 
        inline double getMinAltitude() const { return _minAltitude; }; 
        inline double getMaxAltitude() const { return _maxAltitude; };
        
    private: 

        double _minAltitude, _maxAltitude;
        double _meanRadius; 

};

#endif 