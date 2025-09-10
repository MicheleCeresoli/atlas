#ifndef RENDSETTINGS_H
#define RENDSETTINGS_H

#include "types.h"
#include "raster.h"

#include <cstddef>
#include <string>
#include <vector>
 
enum class LogLevel {
    NONE, 
    MINIMAL, 
    DETAILED
} ;

class SSAAOptions {

    public: 

        size_t nSamples = 4; 
        bool active = true;
        double threshold = 3.0;

        ui16_t boundarySize = 1;
        double resMultiplier = 5;

};


class RenderingOptions {

    public: 

        SSAAOptions ssaa;
        
        size_t gridWidth = 128; 
        size_t gridHeight = 128;

        LogLevel logLevel; 

        bool adaptiveTracing = true;


};


class WorldOptions {

    public: 

        WorldOptions();
        WorldOptions(const RasterDescriptor& dem, const RasterDescriptor& dom); 
        WorldOptions(const std::vector<RasterDescriptor>& dem, const RasterDescriptor& dom);
        WorldOptions(const RasterDescriptor& dem, const std::vector<RasterDescriptor>& dom); 
        WorldOptions(const std::vector<RasterDescriptor>& dem, const std::vector<RasterDescriptor>& dom);

        std::vector<RasterDescriptor> demFiles;
        std::vector<RasterDescriptor> domFiles;

        LogLevel logLevel;

        ui16_t rasterUsageThreshold = 2;

        float minRes = 1;

};

class RayTracerOptions {

    public: 

        RayTracerOptions(size_t nThreads, LogLevel level = LogLevel::MINIMAL);

        void setLogLevel(LogLevel level); 

        size_t nThreads = 1;
        LogLevel logLevel;

        WorldOptions optsWorld;
        RenderingOptions optsRenderer; 

};

#endif