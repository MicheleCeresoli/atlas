
#include "settings.h"



/* -------------------------------------------------------
                        WORLD OPTIONS
---------------------------------------------------------- */

WorldOptions::WorldOptions() : WorldOptions("", "") {}

WorldOptions::WorldOptions(const std::string& dem, const std::string& dom) : 
    WorldOptions(std::vector<std::string>{dem}, std::vector<std::string>{dom}) {}

WorldOptions::WorldOptions(const std::vector<std::string>& dem, const std::string& dom) : 
    WorldOptions(dem, std::vector<std::string>{dom}) {}

WorldOptions::WorldOptions(const std::string& dem, const std::vector<std::string>& dom) : 
    WorldOptions(std::vector<std::string>{dem}, dom) {}

WorldOptions::WorldOptions(
    const std::vector<std::string>& dem, const std::vector<std::string>& dom
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