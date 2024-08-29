#ifndef RENDSETTINGS_H
#define RENDSETTINGS_H

#include <cstddef>
#include <string>
#include <vector>
 

class SSAAOptions {

    public: 

        size_t nSamples = 4; 
        bool active = true;
        double threshold = 3.0;

};


class RenderingOptions {

    public: 

        SSAAOptions ssaa;
        size_t batchSize = 640; 
        bool displayInfo = false;

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

        bool displayInfo = false;

        uint rasterUsageThreshold = 2;

};

class RayTracerOptions {

    public: 

        size_t nThreads = 1;

        WorldOptions optsWorld;
        RenderingOptions optsRenderer; 

};

#endif