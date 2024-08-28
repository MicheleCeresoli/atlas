
#include "renderer.h"
#include "utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <mutex>

// Constructor
Renderer::Renderer(RenderingOptions opts) : 
    pool(ThreadPool(opts.nThreads)), opts(opts)
{
    // Reserve enough space for the this task.
    taskQueue.reserve(opts.batchSize); 

}



// This function stores the output of each render task in the original class
void Renderer::saveRenderTaskOutput(const std::vector<RenderedPixel>& pixels)
{
    {
        std::unique_lock<std::mutex> lock(renderMutex); 
        for (auto p : pixels) {
            if (!hasRendered)
                renderedPixels.push_back(p);
            else 
            {
                renderedPixels[p.id].updateSamples(p.nSamples);
                for (size_t k = 0; k < p.nSamples; k++)
                    renderedPixels[p.id].addPixelData(p.data[k]);
            }
        }
    }
}

// This function renders a batch of pixels
void Renderer::renderTask(
    const ThreadWorker& wk, Camera& cam, World& w, const std::vector<TaskedPixel>& pixels
) {

    // Create a vector storing the pixels to be rendered with the given memory
    std::vector<RenderedPixel> output;
    output.reserve(pixels.size()); 

    for (size_t j = 0; j < pixels.size(); j++)
    {

        RenderedPixel rPix(pixels[j].id, pixels[j].nSamples);
        for (size_t k = 0; k < rPix.nSamples; k++) 
        {
            // Retrieve camera ray for this pixel
            Ray ray = cam.get_ray(pixels[j].u[k], pixels[j].v[k]); 

            // Compute pixel data
            rPix.addPixelData(w.trace_ray(ray, pixels[j].tint, wk.id())); 
        }

        // Add the pixel to the list of computed pixels
        output.push_back(rPix); 
    }

    // Save the rendered pixels in the Rendeder class 
    saveRenderTaskOutput(output); 

}

void Renderer::dispatchTaskQueue(const std::vector<TaskedPixel>& task, Camera& cam, World& w)
{
    pool.addTask(
        [this, &cam, &w, task] (const ThreadWorker& worker) { 
            renderTask(worker, cam, w, task); 
        } 
    );
}

void Renderer::updateTaskQueue(const TaskedPixel& tp, Camera& cam, World& w) {
    
    // Update the task queue
    taskQueue.push_back(tp); 
    if (taskQueue.size() >= opts.batchSize) {
        releaseTaskQueue(cam, w); 
    }
    
}

void Renderer::releaseTaskQueue(Camera& cam, World& w) {
    // Add the task to the thread pool and clear the vector 
    dispatchTaskQueue(taskQueue, cam, w); 
    taskQueue.clear(); 
}

// This function generates all the tasks required to render an image.
uint Renderer::generateRenderTasks(Camera& cam, World& w) {
    
    // Assign all the pixels to a specific rendering task.
    uint nPixels = cam.nPixels();

    uint id, u, v;
    for (id = 0; id < nPixels; id++) {
        // Compute pixel coordinates and update rendering queue
        cam.pixel_coord(id, u, v);
        updateTaskQueue(TaskedPixel(id, u, v), cam, w); 
    }

    /* This could happen whenever the batch-size is not an exact multiple of the number
     * of tasked pixels, risking that the final task is never properly launched. */
    releaseTaskQueue(cam, w); 

    return nPixels;

}

// This function post-processes the outputs of all tasks to generated an 
// orderered list of pixels.
void Renderer::processRenderOutput()
{
    // Update render status
    hasRendered = true; 
    
    // Sort the rendered pixel vector to have increasing pixel IDs; 
    std::sort(renderedPixels.begin(), renderedPixels.end(), 
        [] (const RenderedPixel& p1, const RenderedPixel& p2) { 
            return p1.id < p2.id;
    });
}

uint Renderer::generateAntiAliasingTasks(Camera& cam, World& w) 
{
    // Retrieve the resolution used to propagate the rays
    double rayRes = w.getRayResolution(); 

    // Create an array of pixels that require anti-aliasing.
    std::vector<TaskedPixel> aliasedPixels; 
    aliasedPixels.reserve(renderedPixels.size());  

    uint id, u, v; 
    double tk, tk1, tk2;
    
    bool prev = false; 
    bool aliased = false;

    std::vector<uint8_t> prevCol(cam.height, 0);

    for (id = 0; id < renderedPixels.size(); id++) {

        // Get pixel coordinates
        cam.pixel_coord(id, u, v);

        // Check whether this pixel has already been checked.
        aliased = prev || prevCol[v]; 

        // Retrieve current pixel value 
        tk = renderedPixels[id].pixDistance(); 
        tk1 = tk; tk2 = tk; 

        // Check against the one on the bottom
        if (v < cam.height - 1) {
            tk1 = renderedPixels[id + cam.width].pixDistance();
            prev = isAliased(tk, tk1, rayRes); 
        } 

        // Check against the one on the right
        if (u < cam.width - 1) {
            tk2 = renderedPixels[id + 1].pixDistance();
            prevCol[v] = isAliased(tk, tk2, rayRes); 
        }

        if (aliased || prev || prevCol[v]) {
            // Add the pixel to the SSAA queue and precompute the min\max values
            TaskedPixel tp = TaskedPixel(id, u, v, opts.ssaa.nSamples); 
            tp.tint[0] = std::min({tk, tk1, tk2});
            tp.tint[1] = std::max({tk, tk1, tk2});

            aliasedPixels.push_back(tp);
        }

    }
    
    // Update the aliased pixel t-boundaries
    for (size_t k = 0; k < aliasedPixels.size(); k++) {

        // Retrieve pixel ID and center coordinates 
        id = aliasedPixels[k].id; 
        cam.pixel_coord(id, u, v); 

        tk = renderedPixels[id].pixDistance();

        // Compute the pixel value on the left and on the top
        tk1 = (u > 0) ? renderedPixels[id-1].pixDistance() : tk;
        tk2 = (v > 0) ? renderedPixels[id-cam.width].pixDistance() : tk; 
        
        // Update the pixel t-boundaries
        aliasedPixels[k].tint[0] = std::min({aliasedPixels[k].tint[0], tk1, tk2});
        aliasedPixels[k].tint[1] = std::max({aliasedPixels[k].tint[1], tk1, tk2}); 

        // Add the pixel to the rendering queue
        updateTaskQueue(aliasedPixels[k], cam, w); 

    }

    // This takes care of the batch-size not being a multiplier of the aliased pixels
    releaseTaskQueue(cam, w);

    return aliasedPixels.size(); 
    
}


void Renderer::setupRenderer(const Camera& cam, World& w) {

    // Start the Thread pool, if not started already.
    pool.startPool(); 

    // Compute the ray rendering resolution.
    w.computeRayResolution(cam); 

    hasRendered = false; 

    // Clear the previous output
    renderedPixels.clear();

    // Pre-allocate all the space needed to store all the image pixels.
    renderedPixels.reserve(cam.nPixels()); 

    // Clear the tasked pixels queue 
    taskQueue.clear(); 

}

void Renderer::displayRenderStatus(uint nPixels, std::string m) {

    // Store current time
    auto t1 = std::chrono::high_resolution_clock::now();

    double f = (double)opts.batchSize/nPixels; 

    size_t nTasks = pool.nPendingTasks(); 
    size_t pTasks = nTasks; 

    while (nTasks > 0)
    {
        nTasks = pool.nPendingTasks(); 

        // Update the rendering status only when some tasks are completed.
        if (pTasks != nTasks) {
            pTasks = nTasks;

            std::clog << "\r[" <<  std::setw(3) << int(100*(1 - (double)nTasks*f)) 
                      << "%] \033[32m" + m + " image\033[0m" << std::flush;
        }
        
    }

    // Retrieve time to compute rendering duration
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);
    
    std::clog << "\r[100%] " + m + " completed in " << duration.count() 
              << " seconds." << std::endl; 

}

// This is the high-level function called by the user
std::vector<RenderedPixel> Renderer::render(Camera& cam, World& w, bool displayInfo) {

    // Setup the render output variable.
    setupRenderer(cam, w); 

    // Generate the tasks and add them to the pool (i.e., the list of pixels to render)
    uint nPixels = generateRenderTasks(cam, w); 

    // Display the rendering status
    if (opts.displayInfo) {
        displayRenderStatus(nPixels, "Ray-tracing"); 
    }
    
    // Wait for the completion of all jobs
    pool.waitCompletion();

    // At this point we need to re-order all the rendered pixels.
    processRenderOutput();

    if (opts.ssaa.active) 
    {
        // Run Super-Sampling Antialiasing 
        nPixels = generateAntiAliasingTasks(cam, w); 

        if (opts.displayInfo) {
            displayRenderStatus(nPixels, "Anti-Aliasing"); 
        }

        // Wait for the completion of all jobs
        pool.waitCompletion();
    }

    return renderedPixels;

}

bool Renderer::isAliased(double t1, double t2, double dt) const {
    return fabs(t2-t1) >= opts.ssaa.threshold*dt; 
}