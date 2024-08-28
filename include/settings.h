#ifndef RENDSETTINGS_H
#define RENDSETTINGS_H

#include <cstddef>

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
        size_t nThreads = 1;

        bool displayInfo = false;

};

#endif