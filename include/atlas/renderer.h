#ifndef RENDERER_H 
#define RENDERER_H 

#include "camera.h"
#include "pixel.h"
#include "pool.h"
#include "settings.h"
#include "types.h"
#include "world.h"

#include <mutex>
#include <vector>

enum class RenderingStatus {
    WAITING,
    INITIALISED, 
    TRACING, 
    POST_SSAA, 
    POST_DEFOCUS, 
    COMPLETED
};

class Renderer {
    
    public: 

        Renderer(const RenderingOptions& opts, ui32_t nThreads); 

        void render(const Camera* cam, World& w);

        inline RenderingStatus getStatus() const { return status; }
        inline void updateRenderingOptions(const RenderingOptions& options) {
            opts = options;
        } 

        void importRenderedData(const std::vector<RenderedPixel>& pixels); 

        inline const std::vector<RenderedPixel>* getRenderedPixels() const {
            return &renderedPixels;
        }
        
    private: 

        ThreadPool pool; 
        RenderingOptions opts; 

        RenderingStatus status;

        // Mutex to synchronise access to shared data.
        std::mutex renderMutex; 

        // List storing the output of each render task
        std::vector<RenderedPixel> renderedPixels; 

        // Temporary queue to store the pixels that will be dispatch to the render
        std::vector<TaskedPixel> taskQueue;
        // Vector used to compute the min\max boundaries of each pixel.
        std::vector<TaskedPixel> pixBorders; 

        std::vector<double> pixMaxT; 
        std::vector<double> pixMinT; 

        // Keep track of the total number of pixels to render
        ui32_t nPixels;

        // This function stores the output of each render task in the original class
        void saveRenderTaskOutput(const std::vector<RenderedPixel> &pixels); 
 
        // This function renders a batch of pixels
        void renderTask(
            const ThreadWorker&, const Camera* cam, World& w, 
            const std::vector<TaskedPixel>& pixels
        );

        // Add a rendering task to the thread pool
        void dispatchTaskQueue(
            const std::vector<TaskedPixel>& task, const Camera* cam, World& w
        );

        // Add a pixel to the task queue and dispatch it when batch-size is reached.
        inline void updateTaskQueue(const TaskedPixel& tp) { taskQueue.push_back(tp); } 
        void updateTaskQueue(const TaskedPixel& tp, const Camera* cam, World& w); 

        // Add the task to the thread pool and clear the vector 
        void releaseTaskQueue(const Camera* cam, World& w); 

        // This function generates all the tasks required to render an image.
        void generateRenderTasks(const Camera* cam, World& w);
        void generateBasicRenderTasks(const Camera* cam, World& w);

        void generateAdaptiveRenderTasks(const Camera* cam, World& w);
        void generateRowAdaptiveRenderTasks(const Camera* cam, World& w); 
        void generateColAdaptiveRenderTasks(const Camera* cam, World& w); 

        // Run anti-aliasing on the pixels with a large difference in the distance
        ui32_t generateAntiAliasingTasks(const Camera* cam, World& w);
        ui32_t generateDefocusBlurTasks(const Camera* cam, World& w); 

        void runAntiAliasing(const Camera* cam, World& w);  
        void runDefocusBlur(const Camera* cam, World& w);

        // Dummy pixel rendering when no intersections are detected
        void renderBlack(const Camera* cam); 

        // Update the storing of the rendered pixels
        void setupRenderer(const Camera* cam, World& w); 
        void postProcessRender(const Camera* cam, World& w);  

        // This function post-processes the outputs of all tasks to generated an 
        // orderered list of pixels.
        void sortRenderOutput(); 

        // Retrieve the min\max t-values of each pixel depending on its boundaries
        void computePixelBoundaries(const Camera* cam, ui32_t s);

        // Display the real-time rendering status on the terminal.
        void displayRenderStatus(ui32_t n); 


};

#endif 