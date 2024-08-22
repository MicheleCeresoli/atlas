#ifndef RENDERER_H 
#define RENDERER_H 

#include "pixel.h"
#include "pool.h"
#include "camera.h"
#include "world.h"

#include <mutex>
#include <vector>

class Renderer {
    
    public: 

        Renderer(size_t nThreads, size_t batch_size = 64); 
        std::vector<RenderedPixel> render(Camera& cam, World& w);

    private: 

        ThreadPool pool; 
        size_t batch_size; 

        // Mutex to synchronise access to shared data.
        std::mutex renderMutex; 

        // TODO: this should then be moved to the renderer. 
        std::vector<std::vector<Pixel>> renderTasks; 

        // List storing the output of each render task
        std::vector<std::vector<RenderedPixel>> renderedPixels; 

        // This function stores the output of each render task in the original class
        void saveRenderTaskOutput(const std::vector<RenderedPixel> pixels); 
 
        // This function renders a batch of pixels
        void renderTask(Camera& cam, World& w, const std::vector<Pixel> &pixels);

        // This function generates all the tasks required to render an image.
        void generateRenderTasks(Camera& cam);

        // This function post-processes the outputs of all tasks to generated an 
        // orderered list of pixels.
        std::vector<RenderedPixel> processRenderOutput(Camera& cam); 


};

#endif 