
#include "renderer.h"
#include "utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <mutex>

// Constructor
Renderer::Renderer(const RenderingOptions& opts, uint nThreads) : 
    pool(ThreadPool(nThreads)), opts(opts)
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
    const ThreadWorker& wk, const Camera& cam, World& w, 
    const std::vector<TaskedPixel>& pixels
) {

    // Create a vector storing the pixels to be rendered with the given memory
    std::vector<RenderedPixel> output;
    output.reserve(pixels.size()); 

    double tj, tints[2];

    for (size_t j = 0; j < pixels.size(); j++)
    { 
        tints[1] = pixels[j].tint[1];
        
        // If in adaptive-mode, retrieve the ray starting position from the 
        // previous rendered pixel.
        if ((opts.adaptiveTracing) && (j > 0) && (pixels[j].nSamples == 1)) {
            tints[0] = output[j-1].data[0].t;
            
            /* TODO: Bisogna sistemare anche il valore massimo che gli viene dato. 
             * Vale la pena riflettere un attimo su come si vuole gestire l'intera 
             * catena anche a valle della "Real Camera" che spara raggi a caso. 
             * 
             * Potrebbe aver senso differenziare sin da subito tra il caso in cui spari 
             * per calcolare il "centro del pixel" e quello in cui spari per generare 
             * un immagine realistica, quindi o con aliasing o con defocus blur? */
             
            if (tints[0] == inf) 
                tints[0] = 0;

        } else {
            tints[0] = pixels[j].tint[0];
        }
        
        RenderedPixel rPix(pixels[j].id, pixels[j].nSamples);
        for (size_t k = 0; k < rPix.nSamples; k++) 
        {
            // Retrieve camera ray for this pixel
            Ray ray = cam.get_ray(pixels[j].u[k], pixels[j].v[k]); 
            
            // Compute pixel data
            rPix.addPixelData(w.traceRay(ray, tints, wk.id())); 
        }

        // Add the pixel to the list of computed pixels
        output.push_back(rPix); 
    }

    // Save the rendered pixels in the Rendeder class 
    saveRenderTaskOutput(output); 

}

void Renderer::dispatchTaskQueue(
    const std::vector<TaskedPixel>& task, const Camera& cam, World& w
) {
    pool.addTask(
        [this, &cam, &w, task] (const ThreadWorker& worker) { 
            renderTask(worker, cam, w, task); 
        } 
    );
}

void Renderer::updateTaskQueue(const TaskedPixel& tp, const Camera& cam, World& w) {
    
    // Update the task queue
    updateTaskQueue(tp);
    if (taskQueue.size() >= opts.batchSize) {
        releaseTaskQueue(cam, w); 
    }
    
}

void Renderer::releaseTaskQueue(const Camera& cam, World& w) {
    // Add the task to the thread pool and clear the vector 
    dispatchTaskQueue(taskQueue, cam, w); 
    taskQueue.clear(); 
}

void Renderer::generateRenderTasks(const Camera& cam, World& w) {
    
    if (opts.adaptiveTracing) {
        generateAdaptiveRenderTasks(cam, w);
    } else {
        generateBasicRenderTasks(cam, w);
    } 

}

// This function generates all the tasks required to render an image.
void Renderer::generateBasicRenderTasks(const Camera& cam, World& w) {
    
    // Assign all the pixels to a specific rendering task.

    uint id, u, v;
    for (id = 0; id < nPixels; id++) {
        // Compute pixel coordinates and update rendering queue
        cam.getPixelCoordinates(id, u, v);
        updateTaskQueue(TaskedPixel(id, u, v), cam, w); 
    }

    /* This could happen whenever the batch-size is not an exact multiple of the number
     * of tasked pixels, risking that the final task is never properly launched. */
    releaseTaskQueue(cam, w); 

}

// This function generates all the tasks required to render an image.
void Renderer::generateAdaptiveRenderTasks(const Camera& cam, World& w) {
    
    const std::vector<double>* pRayDistances = w.getRayDistances();

    uint id = 0;
    for (size_t u = 0; u < cam.width(); u++) {
        
        size_t idx_start = u*cam.height(); 
        size_t idx_end   = idx_start + cam.height(); 

        uint min_index = 0;
        for (size_t j = idx_start + 1; j < idx_end; j++) {
            
            // Update current minimum index value
            if ((pRayDistances->at(j-1) != inf) && 
                (pRayDistances->at(j) > pRayDistances->at(j-1))) {
                break;
            }

            min_index++;
        }

        // auto idxStart = pRayDistances->begin() + u*cam.height();
        // auto idxEnd   = idxStart + cam.height(); 
        
        // // Find the iterator to the minimum element
        // auto min_it = std::min_element(idxStart, idxEnd);
        // // Calculate the index
        // int minIndex = std::distance(idxStart, min_it);

        if (min_index == 0) {

            // Here we generate a single task starting from the top of the column
            for (size_t v = 0; v < cam.height(); v++) {
                id = cam.getPixelId(u, v);
                updateTaskQueue(TaskedPixel(id, u, v));
            }

            releaseTaskQueue(cam, w);

        } else if (min_index == cam.height()-1) {

            // Here we generate a single task starting from the bottom of the column
            for (int v = cam.height()-1; v >= 0; v--) {
                id = cam.getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v));
            }

            releaseTaskQueue(cam, w);

        } else {

            /* Here we generate two tasks. The first one goes from the min element to the 
             * top of the column. The second one from the one after the min to the end 
             * of the column. */
            
            for (int v = min_index; v >= 0; v--) {
                id = cam.getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v)); 
            }

            releaseTaskQueue(cam, w);

            for (size_t v = min_index + 1; v < cam.height(); v++) {
                id = cam.getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v)); 
            }

            releaseTaskQueue(cam, w);
        }
    }

}


void Renderer::sortRenderOutput() {
    // Update render status
    hasRendered = true; 
    
    // Sort the rendered pixel vector to have increasing pixel IDs; 
    std::sort(renderedPixels.begin(), renderedPixels.end(), 
        [] (const RenderedPixel& p1, const RenderedPixel& p2) { 
            return p1.id < p2.id;
    });
}

void Renderer::postProcessRender(const PinholeCamera& cam, World& w) {

    if (opts.ssaa.active) 
    {
        // Compute the min\max t-values that should be used for each pixel.
        computePixelBoundaries(cam); 

        // Run Super-Sampling Antialiasing 
        uint nAliased = generateAntiAliasingTasks(cam, w); 

        if (opts.displayInfo) {
            displayRenderStatus(nAliased, "Anti-Aliasing"); 
        }

        // Wait for the completion of all jobs
        pool.waitCompletion();
    }

};

void Renderer::postProcessRender(const RealCamera& cam, World& w) {

    // Or maybe the first one is always from the center of the pixel, 
    // all the others are random...? 

    // Compute the min\max t-values that should be used for each pixel.
    computePixelBoundaries(cam); 

    // Generate all the tasks for the defocus blur
    uint nTasked = generateDefocusBlurTasks(cam, w); 

    if (opts.displayInfo) {
        displayRenderStatus(nTasked, "Defocusing"); 
    }

    // Wait for the completion of all jobs 
    pool.waitCompletion(); 

}


void Renderer::setupRenderer(const Camera& cam, World& w) {

    // Retrieve number of pixels to be rendered
    nPixels = cam.nPixels(); 
    hasRendered = false; 
    
    // Start the Thread pool, if not started already.
    pool.startPool(); 

    // Clear the previous output
    renderedPixels.clear();

    // Pre-allocate all the space needed to store all the image pixels.
    renderedPixels.reserve(nPixels); 

    // Clear the tasked pixels queue 
    taskQueue.clear(); 

    // Clear the pixel boundaries
    pixMinT.clear();
    pixMaxT.clear(); 

    // Reserve space for all pixels
    pixMinT.reserve(nPixels); 
    pixMaxT.reserve(nPixels);

}

void Renderer::displayRenderStatus(uint n, std::string m) {

    // Store current time
    auto t1 = std::chrono::high_resolution_clock::now();

    double f = (double)opts.batchSize/n; 

    size_t nTasks = pool.nPendingTasks(); 
    size_t pTasks = nTasks; 
    size_t nThreads = pool.nThreads();

    while (nTasks > 0)
    {
        nTasks = pool.nPendingTasks(); 

        // Update the rendering status only when some tasks are completed.
        if ((pTasks - nTasks) >= nThreads) {
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
void Renderer::render(const Camera& cam, World& w) {

    // Setup the render output variable.
    setupRenderer(cam, w); 

    // Generate the tasks and add them to the pool (i.e., the list of pixels to render)
    generateRenderTasks(cam, w); 

    // Display the rendering status
    if (opts.displayInfo) {
        displayRenderStatus(nPixels, "Ray-tracing"); 
    }
    
    // Wait for the completion of all jobs
    pool.waitCompletion();

    // At this point we need to re-order all the rendered pixels.
    sortRenderOutput(); 

    // Post process the first rendering depending on the camera type
    processRenderOutput(cam, w);

}

void Renderer::computePixelBoundaries(const Camera& cam) {

    int u, v, ux, vx; 
    int uMin, uMax; 
    int vMin, vMax; 

    uint nBorders;
    size_t idx; 

    double t;
    double tMin, tMax;

    for (size_t id = 0; id < nPixels; id++) {

        // Get pixel coordinates 
        cam.getPixelCoordinates(id, u, v);

        // Go through all pixels 
        uMin = u-1; uMax = u+1;
        vMin = v-1; vMax = v+1;

        if (uMin) < 0 
            uMin = 0;

        if (vMin < 0)
            vMin = 0;

        if (uMax > cam.width-1) 
            uMax = u;

        if (vMax > cam.height-1)
            vMax = v;  

        tMin = inf; tMax = -inf;
        for (size_t j = uMin; j < uMax; j++) {   
            for (size_t k = vMin; k < vMax; k++) {

                // Retrieve new pixel id
                idx = cam.getPixelId(j, ux, vx)
                t = renderedPixels[idx].pixDistance(); 

                tMin = t < tMin ? t : tMin; 
                tMax = t > tMax ? t : tMax; 
            }
        }

        pixMinT.push_back(tMin); 
        pixMaxT.push_back(tMax); 

    }

}


uint Renderer::generateAntiAliasingTasks(const Camera& cam, World& w) {

    // Retrieve current ray resolution.
    double rayRes = w.getRayResolution(); 

    uint u, v; 
    uint nAliased = 0;

    for (size_t id = 0; id < nPixels; id++) {

        // Retrieve pixel coordinates
        cam.getPixelCoordinates(id, u, v); 

        if ((pixMaxT[id] - pixMinT[id]) >= opts.ssaa.threshold*rayRes) {

            // Generate pixel
            TaskedPixel tp(id, u, v, opts.ssaa.nSamples); 

            // Update pixel boundaries
            tp.tint[0] = pixMinT[id]; 
            tp.tint[1] = pixMaxT[id];

            // Add pixel to the rendering queue
            updateTaskQueue(tp, cam, w); 
            nAliased++;  
        }
    }

    // This takes care of the batch-size not being a multiplier of the aliased pixels
    releaseTaskQueue(cam, w); 
    return nAliased;

}

uint Renderer::generateDefocusBlurTasks(const Camera& cam, World& w) {

    uint u, v; 

    for (size_t id = 0; id < nPixels; id++) {
        
        // Retrieve pixel coordinates
        cam.getPixelCoordinates(id, u, v); 

        // Generate pixel 
        TaskedPixel tp(id, u, v, 9); 

        // Update pixel boundaries 
        tp.tint[0] = pixMinT[id];
        tp.tint[1] = pixMaxT[id]; 

        // Add pixel to the rendering queue 
        updateTaskQueue(tp, cam, w); 

    }

    releaseTaskQueue(cam, w); 
    return nPixels;

}