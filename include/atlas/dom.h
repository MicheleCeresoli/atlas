#ifndef DOM_H 
#define DOM_H 

#include "raster.h"
#include "settings.h"
#include "types.h"
#include "vec2.h"

class DOM : public RasterContainer {

    public: 

        DOM(WorldOptions opts, ui32_t nThreads);
        DOM(const std::vector<std::string>& files, ui32_t nThreads, bool displayLogs); 

        double getColor(const point2& s, bool interp, ui32_t threadid = 0); 

};

#endif 