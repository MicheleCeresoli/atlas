#ifndef DOM_H 
#define DOM_H 

#include "raster.h"
#include "vec3.h"


class DOM : public RasterContainer {

    public: 

        DOM(std::string filename, size_t nThreads = 1, bool displayInfo = false);
        DOM(std::vector<std::string> files, size_t nThreads = 1, bool displayInfo = false); 

        double getColor(const point2& s, bool interp, uint threadid = 0) const; 

};

#endif 