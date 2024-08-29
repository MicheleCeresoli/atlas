#ifndef DOM_H 
#define DOM_H 

#include "raster.h"
#include "settings.h"
#include "vec3.h"


class DOM : public RasterContainer {

    public: 

        DOM(std::string filename, RenderingOptions opt);
        DOM(std::vector<std::string> files, RenderingOptions opt); 

        double getColor(const point2& s, bool interp, uint threadid = 0); 

};

#endif 