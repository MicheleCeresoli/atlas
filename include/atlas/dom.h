#ifndef DOM_H 
#define DOM_H 

#include "raster.h"
#include "settings.h"
#include "types.h"
#include "vec2.h"

class DOM : public RasterManager {

    public: 

        DOM(WorldOptions opts, ui32_t nThreads);
        DOM(const std::vector<RasterDescriptor>& files, ui32_t nThreads, bool displayLogs); 

        double getColor(const point2& s, double res, ui32_t threadid = 0); 

};

#endif 