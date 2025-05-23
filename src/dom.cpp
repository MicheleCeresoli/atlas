
#include "dom.h"

DOM::DOM(const std::vector<RasterDescriptor>& files, ui32_t nThreads, bool displayLogs) : 
    RasterContainer(files, nThreads, displayLogs) {}

DOM::DOM(WorldOptions opts, ui32_t nThreads) : 
    DOM(opts.domFiles, nThreads, opts.logLevel >= LogLevel::MINIMAL) {}

double DOM::getColor(const point2& s, bool interp, ui32_t tid) {
    double c = getData(s, interp, tid); 
    return c > 0.0 ? c : 0.0;
}