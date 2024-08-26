
#include "renderer.h"

#include <algorithm>
#include <cmath>
#include <mutex>

// Constructor
Renderer::Renderer(size_t nThreads, size_t batch_size) : 
    pool(ThreadPool(nThreads)), batch_size(batch_size) {}



// This function stores the output of each render task in the original class
void Renderer::saveRenderTaskOutput(const std::vector<RenderedPixel> &pixels)
{
    {
        std::unique_lock<std::mutex> lock(renderMutex); 
        for (int j = 0; j < pixels.size(); j++)
        {
            renderedPixels.push_back(pixels[j]); 
        }
    }
}

// This function renders a batch of pixels
void Renderer::renderTask(
    const ThreadWorker& wk, Camera& cam, World& w, const std::vector<Pixel> &pixels
) {

    // Create a vector storing the pixels to be rendered with the given memory
    std::vector<RenderedPixel> output;
    output.reserve(pixels.size()); 

    RenderedPixel rPix; 

    for (int j = 0; j < pixels.size(); j++)
    {
        Pixel pix = pixels[j]; 
        rPix = {.p = pix}; 
        
        // Retrieve camera ray for this pixel
        Ray ray = cam.get_ray(pix.u, pix.v); 

        // Compute pixel data
        rPix.d = w.trace_ray(ray, wk.id()); 

        // Add the pixel to the list of computed pixels
        output.push_back(rPix); 

    }

    // Save the rendered pixels in the Rendeder class 
    saveRenderTaskOutput(output); 

}

void Renderer::dispatchTask(Camera& cam, World& w, const std::vector<Pixel> &task)
{
    pool.addTask(
        [this, &cam, &w, task] (const ThreadWorker& worker) { 
            renderTask(worker, cam, w, task); 
        } 
    );
}

// This function generates all the tasks required to render an image.
void Renderer::generateRenderTasks(Camera& cam, World& w) {
    
    // List of pixels for each task
    std::vector<Pixel> task;
    task.reserve(batch_size); 

    Pixel pix; 

    // // Compute the total number of pixels that must be rendered. 
    // int nPixels = cam.width * cam.height; 
    // int nTasks = (int) ceil((double)nPixels / batch_size); 

    // Assign all the pixels to a specific rendering task.
    int j = 0, i = 0;
    while (j < cam.height)
    {
        if (i >= cam.width)
        {
            j++; 
            i = 0; 
        } 

        pix = {.u = i, .v = j};
        task.push_back(pix);  

        if (task.size() >= batch_size)
        {
            // Add the task to the thread pool
            dispatchTask(cam, w, task);
            task.clear(); 
        }

        i++; 
    }

}

// This function post-processes the outputs of all tasks to generated an 
// orderered list of pixels.
void Renderer::processRenderOutput(Camera& cam)
{
    // Sort the rendered pixel vector to have increasing pixel IDs; 
    std::sort(renderedPixels.begin(), renderedPixels.end(), 
        [&cam] (const RenderedPixel &p1, const RenderedPixel &p2) { 
            return cam.pixel_id(p1.p) < cam.pixel_id(p2.p);
    });

}

void Renderer::setupRenderOutput(Camera& cam) {

    // Clear the previous output
    renderedPixels.clear();

    // Pre-allocate all the space needed to store all the image pixels.
    int nPixels = cam.width * cam.height; 
    renderedPixels.reserve(nPixels); 

}


// This is the high-level function called by the user
std::vector<RenderedPixel> Renderer::render(Camera& cam, World& w) {

    // Start the Thread pool, if not started already.
    pool.startPool(); 

    // Setup the render output variable.
    setupRenderOutput(cam); 

    // Generate the tasks and add them to the pool (i.e., the list of pixels to render)
    generateRenderTasks(cam, w); 

    // Wait until all the render tasks are completed.
    pool.waitCompletion();

    // At this point we need to re-order all the rendered pixels.
    processRenderOutput(cam);

    return renderedPixels;

}

