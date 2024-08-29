#ifndef RENDERER_H 
#define RENDERER_H 

#include "camera.h"
#include "pixel.h"
#include "pool.h"
#include "settings.h"
#include "world.h"

#include <mutex>
#include <vector>

class Renderer {
    
    public: 

        Renderer(const RenderingOptions& opts, uint nThreads); 

        void render(const Camera& cam, World& w);

        inline const std::vector<RenderedPixel>* getRenderedPixels() const {
            return &renderedPixels;
        }
        

    private: 

        ThreadPool pool; 
        RenderingOptions opts; 

        // Mutex to synchronise access to shared data.
        std::mutex renderMutex; 

        // List storing the output of each render task
        std::vector<RenderedPixel> renderedPixels; 

        // Temporary queue to store the pixels that will be dispatch to the render
        std::vector<TaskedPixel> taskQueue;

        bool hasRendered;

        // This function stores the output of each render task in the original class
        void saveRenderTaskOutput(const std::vector<RenderedPixel> &pixels); 
 
        // This function renders a batch of pixels
        void renderTask(
            const ThreadWorker&, const Camera& cam, World& w, const std::vector<TaskedPixel>& pixels
        );

        // Add a rendering task to the thread pool
        void dispatchTaskQueue(const std::vector<TaskedPixel>& task, const Camera& cam, World& w);

        // Add a pixel to the task queue and dispatch it when batch-size is reached.
        void updateTaskQueue(const TaskedPixel& tp, const Camera& cam, World& w); 

        // Add the task to the thread pool and clear the vector 
        void releaseTaskQueue(const Camera& cam, World& w); 

        // This function generates all the tasks required to render an image.
        uint generateRenderTasks(const Camera& cam, World& w);

        // Run anti-aliasing on the pixels with a large difference in the distance
        uint generateAntiAliasingTasks(const Camera& cam, World& w);

        // Update the storing of the rendered pixels
        void setupRenderer(const Camera& cam, World& w); 

        // This function post-processes the outputs of all tasks to generated an 
        // orderered list of pixels.
        void processRenderOutput(); 

        // Display the real-time rendering status on the terminal.
        void displayRenderStatus(uint nPixels, std::string m); 

        // Check whether two pixel values are aliased. 
        bool isAliased(double t1, double t2, double dt) const; 

};

#endif 