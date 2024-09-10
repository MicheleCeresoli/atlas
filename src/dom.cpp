
#include "dom.h"

DOM::DOM(const std::vector<std::string>& files, uint nThreads, bool displayLogs) : 
    RasterContainer(files, nThreads, displayLogs) {}

DOM::DOM(WorldOptions opts, uint nThreads) : 
    DOM(opts.domFiles, nThreads, opts.logLevel >= LogLevel::MINIMAL) {}

double DOM::getColor(const point2& s, bool interp, uint tid) {
    double c = getData(s, interp, tid); 
    return c > 0.0 ? c : 0.0;
}