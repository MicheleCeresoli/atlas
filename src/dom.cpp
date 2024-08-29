
#include "dom.h"

DOM::DOM(std::vector<std::string> files, RenderingOptions opts) : 
    RasterContainer(files, opts.nThreads, opts.displayInfo) {}

DOM::DOM(std::string filename, RenderingOptions opts) : 
    DOM(std::vector<std::string>{filename}, opts) {}


double DOM::getColor(const point2& s, bool interp, uint tid) {
    double c = getData(s, interp, tid); 
    return c > 0.0 ? c : 0.0;
}