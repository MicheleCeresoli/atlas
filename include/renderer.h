#ifndef RENDERER_H 
#define RENDERER_H 

#include "pixel.h"
#include "pool.h"
#include "camera.h"
#include "world.h"

#include <vector>

class Renderer {
    
    public: 

        Renderer(size_t nThreads, size_t batch_size = 64); 
        std::vector<RenderedPixel> render(Camera& cam, World& w);

    private: 

        ThreadPool pool; 
        size_t batch_size; 

        std::vector<RenderedPixel> 
        renderTask(Camera& cam, World& w, const std::vector<Pixel> pixels);


};

#endif 