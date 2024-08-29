#ifndef DOM_H 
#define DOM_H 

#include "raster.h"
#include "settings.h"
#include "vec2.h"

class DOM : public RasterContainer {

    public: 

        DOM(WorldOptions opts, uint nThreads);
        DOM(const std::vector<std::string>& files, uint nThreads, bool displayInfo); 

        double getColor(const point2& s, bool interp, uint threadid = 0); 

};

#endif 