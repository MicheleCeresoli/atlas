#ifndef RENDSETTINGS_H
#define RENDSETTINGS_H

#include "types.h"

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
        size_t batchSize = 640; 

        LogLevel logLevel; 

        bool adaptiveTracing = true;


};


class WorldOptions {

    public: 

        WorldOptions();
        WorldOptions(const std::string& dem, const std::string& dom); 
        WorldOptions(const std::vector<std::string>& dem, const std::string& dom);
        WorldOptions(const std::string& dem, const std::vector<std::string>& dom); 
        WorldOptions(const std::vector<std::string>& dem, const std::vector<std::string>& dom);

        std::vector<std::string> demFiles; 
        std::vector<std::string> domFiles;

        LogLevel logLevel;

        ui16_t rasterUsageThreshold = 2;

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