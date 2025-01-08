
#include "settings.h"



/* -------------------------------------------------------
                        WORLD OPTIONS
---------------------------------------------------------- */

WorldOptions::WorldOptions() : WorldOptions(RasterDescriptor(), RasterDescriptor()) {}

WorldOptions::WorldOptions(const RasterDescriptor& dem, const RasterDescriptor& dom) : 
    WorldOptions(std::vector<RasterDescriptor>{dem}, std::vector<RasterDescriptor>{dom}) {}

WorldOptions::WorldOptions(const std::vector<RasterDescriptor>& dem, const RasterDescriptor& dom) : 
    WorldOptions(dem, std::vector<RasterDescriptor>{dom}) {}

WorldOptions::WorldOptions(const RasterDescriptor& dem, const std::vector<RasterDescriptor>& dom) : 
    WorldOptions(std::vector<RasterDescriptor>{dem}, dom) {}

WorldOptions::WorldOptions(
    const std::vector<RasterDescriptor>& dem, const std::vector<RasterDescriptor>& dom
) : demFiles(dem), domFiles(dom) {}



/* -------------------------------------------------------
                        RAY TRACER OPTIONS
---------------------------------------------------------- */

RayTracerOptions::RayTracerOptions(size_t nThreads, LogLevel level) : 
    nThreads(nThreads), logLevel(level) {

    // Set the inner log levels 
    optsWorld.logLevel = level; 
    optsRenderer.logLevel = level;

}

void RayTracerOptions::setLogLevel(LogLevel level) {

    // Update all the log levels.
    logLevel = level; 
    optsWorld.logLevel = level; 
    optsRenderer.logLevel = level;

}