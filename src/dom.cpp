
#include "dom.h"

DOM::DOM(std::vector<std::string> files, size_t nThreads, bool displayInfo) : 
    RasterContainer(files, nThreads, displayInfo) {}

DOM::DOM(std::string filename, size_t nThreads, bool displayInfo) : 
    DOM(std::vector<std::string>{filename}, nThreads, displayInfo) {}


double DOM::getColor(const point2& s, bool interp, uint tid) const {
    double c = getData(s, interp, tid); 
    return c > 0.0 ? c : 0.0;
}