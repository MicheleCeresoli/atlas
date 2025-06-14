
#include "dom.h"

DOM::DOM(const std::vector<RasterDescriptor>& files, ui32_t nThreads, bool displayLogs) : 
    RasterManager(files, nThreads, displayLogs) {}

DOM::DOM(WorldOptions opts, ui32_t nThreads) : 
    DOM(opts.domFiles, nThreads, opts.logLevel >= LogLevel::MINIMAL) {}

double DOM::getColor(const point2& s, double res, ui32_t tid) {
    double c = getData(s, res, tid); 
    return c > 0.0 ? c : 0.0;
}