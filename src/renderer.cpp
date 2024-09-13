
#include "renderer.h"
#include "utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <mutex>

// Constructor
Renderer::Renderer(const RenderingOptions& opts, ui16_t nThreads) : 
    pool(ThreadPool(nThreads)), opts(opts)
{
    // Reserve enough space for the this task.
    taskQueue.reserve(opts.batchSize); 

    // Update status
    status = RenderingStatus::WAITING;

}

// This function stores the output of each render task in the original class
void Renderer::saveRenderTaskOutput(const std::vector<RenderedPixel>& pixels)
{
    {
        std::unique_lock<std::mutex> lock(renderMutex); 
        for (auto p : pixels) {
            if (status >= RenderingStatus::TRACING)
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
    const ThreadWorker& wk, const Camera* cam, World& w, 
    const std::vector<TaskedPixel>& pixels
) {

    // Create a vector storing the pixels to be rendered with the given memory
    std::vector<RenderedPixel> output;
    output.reserve(pixels.size()); 

    // Pixel center coordinates
    ui16_t u, v; 
    // Minimum/maximum t-values for the ray-tracing
    double tMin, tMax = inf;
    // Variable to keep track of the minimum distance reached. 
    double tStart = 0;
    // True if the ray should be shot from the pixel center (used only in Pinhole)
    bool center;
    // Get ray resolution 
    double dt = w.getRayResolution();

    for (size_t j = 0; j < pixels.size(); j++)
    { 
        // Initialise the pixel to be rendered.
        RenderedPixel rPix(pixels[j]);

        if (status == RenderingStatus::TRACING) {

            // tMax = inf; 
            center = true;

            if (opts.adaptiveTracing && j > 0 && tStart != inf) {
                tMin = tStart - 5*dt;
            } else {
                tMin = 0.0; 
            }
        } else {

            // Update the pixel boundaries
            center = false; 
            tMin = pixels[j].tMin - opts.ssaa.resMultiplier*dt; 
            // tMax = pixels[j].tMax + opts.ssaa.resMultiplier*dt;
        }

        for (size_t k = 0; k < rPix.nSamples; k++) 
        {
            // Retrieve camera ray for this pixel
            Ray ray = cam->getRay(pixels[j].u[k], pixels[j].v[k], center); 
            
            // Compute pixel data
            rPix.addPixelData(w.traceRay(ray, tMin, tMax, wk.id())); 
        }

        // Add the pixel to the list of computed pixels
        output.push_back(rPix); 

        // Update the minimum distance reached in the last pixel
        tStart = rPix.pixMinDistance(); 
    }

    // Save the rendered pixels in the Rendeder class 
    saveRenderTaskOutput(output); 

}

void Renderer::dispatchTaskQueue(
    const std::vector<TaskedPixel>& task, const Camera* cam, World& w
) {
    pool.addTask(
        [this, cam, &w, task] (const ThreadWorker& worker) { 
            renderTask(worker, cam, w, task); 
        } 
    );
}

void Renderer::updateTaskQueue(const TaskedPixel& tp, const Camera* cam, World& w) {
    
    // Update the task queue
    updateTaskQueue(tp);
    if (taskQueue.size() >= opts.batchSize) {
        releaseTaskQueue(cam, w); 
    }
    
}

void Renderer::releaseTaskQueue(const Camera* cam, World& w) {
    // Add the task to the thread pool and clear the vector 
    if (taskQueue.size() > 0) {
        dispatchTaskQueue(taskQueue, cam, w); 
        taskQueue.clear(); 
    }
}

void Renderer::generateRenderTasks(const Camera* cam, World& w) {
    
    // Update current rendering status
    status = RenderingStatus::TRACING;

    if (opts.adaptiveTracing) {
        generateAdaptiveRenderTasks(cam, w);
    } else {
        generateBasicRenderTasks(cam, w);
    } 

}

// This function generates all the tasks required to render an image.
void Renderer::generateBasicRenderTasks(const Camera* cam, World& w) {
    
    // Assign all the pixels to a specific rendering task.

    ui16_t u, v;
    for (ui32_t id = 0; id < nPixels; id++) {
        // Compute pixel coordinates and update rendering queue
        cam->getPixelCoordinates(id, u, v);
        updateTaskQueue(TaskedPixel(id, u, v), cam, w); 
    }

    /* This could happen whenever the batch-size is not an exact multiple of the number
     * of tasked pixels, risking that the final task is never properly launched. */
    releaseTaskQueue(cam, w); 

}

void Renderer::generateColAdaptiveRenderTasks(const Camera* cam, World& w) {

    if (opts.logLevel >= LogLevel::DETAILED) {
        displayTime(); 
        std::clog << "Selected Adaptive Column Tracing." << std::endl;
    }

    const std::vector<double>* pRayDistances = w.getRayDistances();

    ui32_t id;
    for (size_t u = 0; u < cam->width(); u++) {
        
        ui16_t min_index = 0; 
        for (size_t v = 1; v < cam->height(); v++) {

            size_t vp = v - 1 + u*cam->height();
            size_t vn = vp + 1;

            // Update current minimum index value
            if ((pRayDistances->at(vp) != inf) && 
                (pRayDistances->at(vn) > pRayDistances->at(vp))) {
                break;
            }

            min_index++;
        }
        
        if (min_index == 0) {
        
            // Here we generate a single task starting from the top of the column
            for (size_t v = 0; v < cam->height(); v++) {
                id = cam->getPixelId(u, v);
                updateTaskQueue(TaskedPixel(id, u, v));
            }

            releaseTaskQueue(cam, w);

        } else if (min_index == cam->height()-1) {

            // Here we generate a single task starting from the bottom of the column
            for (int v = cam->height()-1; v >= 0; v--) {
                id = cam->getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v));
            }

            releaseTaskQueue(cam, w);

        } else {

            /* Here we generate two tasks. The first one goes from the min element to the 
             * top of the column. The second one from the one after the min to the end 
             * of the column. */
            
            for (int v = min_index; v >= 0; v--) {
                id = cam->getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v)); 
            }

            releaseTaskQueue(cam, w);

            for (size_t v = min_index + 1; v < cam->height(); v++) {
                id = cam->getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v)); 
            }

            releaseTaskQueue(cam, w);
        }
    }

}

void Renderer::generateRowAdaptiveRenderTasks(const Camera* cam, World& w) {

    if (opts.logLevel >= LogLevel::DETAILED) {
        displayTime(); 
        std::clog << "Selected Adaptive Row Tracing." << std::endl;
    }

    const std::vector<double>* pRayDistances = w.getRayDistances();

    ui32_t id;
    for (size_t v = 0; v < cam->height(); v++) {
        
        ui16_t min_index = 0; 
        for (size_t u = 1; u < cam->width(); u++) {

            size_t up = v + (u-1)*cam->height();
            size_t un = up + cam->height();

            // Update current minimum index value
            if ((pRayDistances->at(up) != inf) && 
                (pRayDistances->at(un) > pRayDistances->at(up))) {
                break;
            }

            min_index++;
        }
        
        if (min_index == 0) {
        
            // Here we generate a single task starting from the left of the row
            for (size_t u = 0; u < cam->width(); u++) {
                id = cam->getPixelId(u, v);
                updateTaskQueue(TaskedPixel(id, u, v));
            }

            releaseTaskQueue(cam, w);

        } else if (min_index == cam->width()-1) {

            // Here we generate a single task starting from the right of the row
            for (int u = cam->width()-1; u >= 0; u--) {
                id = cam->getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v));
            }

            releaseTaskQueue(cam, w);

        } else {

            /* Here we generate two tasks. The first one goes from the min element to the 
             * left of the row. The second one from the one after the min to the end 
             * of the row. */
            
            for (int u = min_index; u >= 0; u--) {
                id = cam->getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v)); 
            }

            releaseTaskQueue(cam, w);

            for (size_t u = min_index + 1; u < cam->width(); u++) {
                id = cam->getPixelId(u, v); 
                updateTaskQueue(TaskedPixel(id, u, v)); 
            }

            releaseTaskQueue(cam, w);
        }
    }
}

// This function generates all the tasks required to render an image.
void Renderer::generateAdaptiveRenderTasks(const Camera* cam, World& w) {

    /* The idea is the following: we need to find out whether the variation along 
     * the ray distances is greater along the rows or the columns. This allows to avoid 
     * issues when all the pixels are at more or less the same distance causing the 
     * actual closest pixel to be further from its neighbours when the DEM is used. */
    
    const std::vector<double>* pRayDistances = w.getRayDistances();

    double cMeanDist = 0, rMeanDist = 0; 
    double dMin, dMax, dj;
    
    size_t u, v; 

    // Check the average distance along the columns 
    for (u = 0; u < cam->width(); u++) {

        dMin =  inf; 
        dMax = -inf; 
        
        for (v = 0; v < cam->height(); v++) {
            dj = pRayDistances->at(v + u*cam->height());

            // Update the min\maximum distances 
            if (dj != inf) {
                dMin = dj < dMin ? dj : dMin; 
                dMax = dj > dMax ? dj : dMax;
            }
        }

        if (dMax > 0.0) {
            cMeanDist += dMax - dMin; 
        }
    }

    // Check the average distance along the rows
    for (v = 0; v < cam->height(); v++) {
        
        dMin =  inf; 
        dMax = -inf; 

        for (u = 0; u < cam->width(); u++) {
            dj = pRayDistances->at(v + u*cam->height());

            // Update the min\maximum distances 
            if (dj != inf) {
                dMin = dj < dMin ? dj : dMin; 
                dMax = dj > dMax ? dj : dMax;
            }
        }

        if (dMax > 0.0) {
            rMeanDist += dMax - dMin; 
        }
    }

    // Average the distance between the number of columns\rows
    cMeanDist /= cam->width();
    rMeanDist /= cam->height();

    // User rows if the distance excursion along the rows is greater than along the columns
    if (rMeanDist > cMeanDist) {
        generateRowAdaptiveRenderTasks(cam, w);
    } else {
        generateColAdaptiveRenderTasks(cam, w);
    }

}


void Renderer::sortRenderOutput() {
    // Sort the rendered pixel vector to have increasing pixel IDs; 
    std::sort(renderedPixels.begin(), renderedPixels.end(), 
        [] (const RenderedPixel& p1, const RenderedPixel& p2) { 
            return p1.id < p2.id;
    });
}

void Renderer::runAntiAliasing(const Camera* cam, World& w) {
    
    if (opts.ssaa.active) 
    {
        // Update rendering status
        status = RenderingStatus::POST_SSAA; 

        // Compute the min\max t-values that should be used for each pixel.
        computePixelBoundaries(cam, opts.ssaa.boundarySize); 

        // Run Super-Sampling Antialiasing 
        ui32_t nAliased = generateAntiAliasingTasks(cam, w); 

        // Display status if required.
        displayRenderStatus(nAliased); 

        // Wait for the completion of all jobs
        pool.waitCompletion();
    }

};

void Renderer::runDefocusBlur(const Camera* cam, World& w) {

    // Update rendering status
    status = RenderingStatus::POST_DEFOCUS;

    // Compute the min\max t-values that should be used for each pixel.
    // TODO: this value should probably be increased...
    computePixelBoundaries(cam, 3); 

    // Generate all the tasks for the defocus blur
    ui32_t nTasked = generateDefocusBlurTasks(cam, w); 

    // Display status if required.
    displayRenderStatus(nTasked); 

    // Wait for the completion of all jobs 
    pool.waitCompletion(); 

}

void Renderer::postProcessRender(const Camera* cam, World& w) {

    // Perform SSAA
    if (cam->hasAntiAliasing()) {
        runAntiAliasing(cam, w);
    }

    // Add defocus blur effect
    if (cam->hasDefocusBlur()) {
        runDefocusBlur(cam, w);
    }

    // Update rendering status
    status = RenderingStatus::COMPLETED;

}


void Renderer::setupRenderer(const Camera* cam, World& w) {

    // Retrieve number of pixels to be rendered
    nPixels = cam->nPixels(); 
    
    // Start the Thread pool, if not started already.
    pool.startPool(); 

    // Clear the previous output
    renderedPixels.clear();

    // Pre-allocate all the space needed to store all the image pixels.
    renderedPixels.reserve(nPixels); 

    // Clear the tasked pixels queue 
    taskQueue.clear(); 

    // Reserve space for all pixels
    pixMinT.reserve(nPixels); 
    pixMaxT.reserve(nPixels);

    // Update status
    status = RenderingStatus::INITIALISED;

}

void Renderer::displayRenderStatus(ui32_t n) {

    if (opts.logLevel < LogLevel::DETAILED)
        return; 

    // Store current time
    auto t1 = std::chrono::high_resolution_clock::now();

    size_t nTasks = pool.nPendingTasks();
    size_t pTasks = nTasks;
    size_t nThreads = pool.nThreads();

    double f = 1.0/nTasks;

    // Set string to be displayed 
    std::string m; 
    switch (status) {

        case RenderingStatus::TRACING: 
            m = "Ray-tracing";
            break; 

        case RenderingStatus::POST_SSAA:
            m = "Antialiasing (SSAA)";
            break; 

        case RenderingStatus::POST_DEFOCUS: 
            m = "Defocusing"; 
            break; 
    }

    while (nTasks > 0)
    {
        nTasks = pool.nPendingTasks(); 

        // Update the rendering status only when some tasks are completed.
        if ((pTasks - nTasks) >= nThreads) {
            pTasks = nTasks;

            displayTime(); 
            std::clog << "\033[32m[\033[1;32m" <<  std::setw(3) 
                      << int(100*(1 - pTasks*f)) 
                      << "%\033[0;32m] " + m + " image\033[0m" << std::flush;
        }
        
    }

    // Retrieve time to compute rendering duration
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);
    
    displayTime(); 
    std::clog << m + " completed in " << duration.count() << " seconds. " << std::endl; 

}

// This is the high-level function called by the user
void Renderer::render(const Camera* cam, World& w) {

    // Setup the render output variable.
    setupRenderer(cam, w); 

    // Generate the tasks and add them to the pool (i.e., the list of pixels to render)
    generateRenderTasks(cam, w); 

    // Display the rendering status if required.
    displayRenderStatus(nPixels); 
    
    // Wait for the completion of all jobs
    pool.waitCompletion();

    // At this point we need to re-order all the rendered pixels.
    sortRenderOutput(); 

    // Post process the first rendering depending on the camera type
    postProcessRender(cam, w);
     
    if (opts.logLevel >= LogLevel::MINIMAL) {
        displayTime(); 
        std::clog << "Rendering process completed." << std::endl;
    }

}

void Renderer::computePixelBoundaries(const Camera* cam, ui16_t s) {
    
    // Clear the pixel boundaries
    pixMinT.clear();
    pixMaxT.clear(); 

    int uMin, uMax; 
    int vMin, vMax; 

    ui16_t u, v; 
    size_t idx; 

    double t1, t2;
    double tMin, tMax;

    for (size_t id = 0; id < nPixels; id++) {

        // Get pixel coordinates 
        cam->getPixelCoordinates(id, u, v);

        // Go through all pixels 
        uMin = u-s; uMax = u+s;
        vMin = v-s; vMax = v+s;

        if (uMin < 0) 
            uMin = 0;

        if (vMin < 0)
            vMin = 0;

        if (uMax > cam->width()-s) 
            uMax = cam->width()-s;

        if (vMax > cam->height()-s)
            vMax = cam->height()-s;  

        tMin = inf; tMax = -inf;
        for (size_t j = uMin; j < uMax; j++) {   
            for (size_t k = vMin; k < vMax; k++) {

                // Retrieve new pixel id
                idx = cam->getPixelId(j, k);

                t1 = renderedPixels[idx].pixMinDistance(); 
                t2 = renderedPixels[idx].pixMaxDistance();
                
                tMin = t1 < tMin ? t1 : tMin; 
                tMax = t2 > tMax ? t2 : tMax; 
            }
        }

        pixMinT.push_back(tMin); 
        pixMaxT.push_back(tMax); 

    }

}


ui32_t Renderer::generateAntiAliasingTasks(const Camera* cam, World& w) {

    // Retrieve current ray resolution.
    double rayRes = w.getRayResolution(); 

    ui16_t u, v; 
    ui32_t nAliased = 0;

    for (size_t id = 0; id < nPixels; id++) {

        // Retrieve pixel coordinates
        cam->getPixelCoordinates(id, u, v); 

        if ((pixMaxT[id] - pixMinT[id]) >= opts.ssaa.threshold*rayRes) {

            // Generate pixel
            TaskedPixel tp(id, u, v, opts.ssaa.nSamples);
            // Compute SSAA sampling points
            updateSSAACoordinates(tp);

            // Update pixel boundaries
            tp.tMin = pixMinT[id]; 
            tp.tMax = pixMaxT[id];

            // Add pixel to the rendering queue
            updateTaskQueue(tp, cam, w); 
            nAliased++;  
        }
    }

    // This takes care of the batch-size not being a multiplier of the aliased pixels
    releaseTaskQueue(cam, w); 
    return nAliased;

}

ui32_t Renderer::generateDefocusBlurTasks(const Camera* cam, World& w) {

    ui16_t u, v; 
    for (size_t id = 0; id < nPixels; id++) {
        
        // Retrieve pixel coordinates
        cam->getPixelCoordinates(id, u, v); 

        // Generate pixel 
        TaskedPixel tp(id, u, v, 9); 

        // Update pixel boundaries 
        tp.tMin = pixMinT[id];
        tp.tMax = pixMaxT[id]; 

        // Add pixel to the rendering queue 
        updateTaskQueue(tp, cam, w); 

    }

    releaseTaskQueue(cam, w); 
    return nPixels;

}


void Renderer::importRenderedData(const std::vector<RenderedPixel>& pixels) {

    // Copy the content 
    renderedPixels = pixels;

    // Update the rendering status
    status = RenderingStatus::COMPLETED;

}