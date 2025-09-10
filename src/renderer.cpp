
#include "renderer.h"
#include "utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <mutex>

// Constructor
Renderer::Renderer(const RenderingOptions& opts, ui32_t nThreads) : 
    pool(ThreadPool(nThreads)), opts(opts)
{
    // Reserve enough space for the this task.
    taskQueue.reserve(MIN(opts.gridHeight, opts.gridWidth)); 

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
    ui32_t u, v; 
    // Minimum/maximum t-values for the ray-tracing
    double tMin, tMax = inf;
    // Variable to keep track of the minimum distance reached. 
    double tStart = 0;
    // True if the ray should be shot from the pixel center (used only in Pinhole)
    bool center;
    double dt;

    for (size_t j = 0; j < pixels.size(); j++)
    { 
        // Initialise the pixel to be rendered.
        RenderedPixel rPix(pixels[j]);

        // Store the pixel resolution 
        dt = pixels[j].dt;

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
            rPix.addPixelData(w.traceRay(ray, dt, tMin, tMax, wk.id())); 
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
    if (taskQueue.size() >= MIN(opts.gridHeight, opts.gridWidth)) {
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

    // Initialise the screen grids subdivision and get the max resolution
    double maxRes = initializeGrids(cam, w);
    
    // Check whether something has to be rendered on screen
    if (std::isinf(maxRes)) {
        renderBlack(cam); 
        return;
    }

    double minRes  = inf; 
    double meanRes = 0; 
    double dt; 

    size_t nGrids = 0, nRows = 0, nCols = 0;

    /* Iterate over each grid. */
    for (ScreenGrid grid : grids) {

        /* If a grid does not exhibit any interesection, we set its resolution to the 
         * maximum used in the image. */
        if (std::isinf(grid.getRayResolution())) {
            grid.setRayResolution(maxRes);
        }

        if (opts.adaptiveTracing) {

            // Dispatch adaptive rendering tasks.
            if (grid.isRowAdaptiveRendering()) {
                generateRowAdaptiveRenderTasks(grid, cam, w);
                nRows += 1;
            } 
            else {
                generateColAdaptiveRenderTasks(grid, cam, w);
                nCols += 1;
            }
            
        } 
        else {
            generateBasicRenderTasks(grid, cam, w);
        }

        // Update the resolution statistics
        dt = grid.getRayResolution();
        if (dt != inf) {
            meanRes += dt;
            nGrids += 1;

            minRes = (dt < minRes) ? dt : minRes; 
            maxRes = (dt > maxRes) ? dt : maxRes;
        }
    }

    // Display the average ray resolution.
    if (opts.logLevel >= LogLevel::DETAILED) {
        displayTime(); 

        if (nGrids == 0) {
            std::clog << "No valid ray intersection detected." << std::endl; 
            return;
        } 
        else {

            meanRes /= nGrids;
            std::clog << "Grid ray resolutions (min/mean/max): "
                      << "\033[35m" << int(floor(minRes))  << "m" << "\033[0m/" 
                      << "\033[35m" << int(floor(meanRes)) << "m" << "\033[0m/" 
                      << "\033[35m" << int(floor(maxRes))  << "m" << "\033[0m" << std::endl;
        }

        if (opts.adaptiveTracing) {
            displayTime(); 
            std::clog << "Adaptive tracing with " << nRows << " rows and " 
                      << nCols << " columns." << std::endl;
        }
    }

}

// This function generates all the tasks required to render an image.
void Renderer::generateBasicRenderTasks(
    const ScreenGrid& grid, const Camera* cam, World& w
) {

    // Retrieve the ray resolution 
    double dt = grid.getRayResolution();

    ui32_t id, u, v;
    for (ui32_t gid = 0; gid < grid.nPixels(); gid++) {

        // Compute pixel coordinates and ID with respect to the camera
        grid.getGPixelCoordinates(gid, u, v);
        grid.getGPixelId(gid);

        updateTaskQueue(TaskedPixel(id, u, v, dt), cam, w); 

    }

    /* This could happen whenever the batch-size is not an exact multiple of the number
     * of tasked pixels, risking that the final task is never properly launched. */
    releaseTaskQueue(cam, w); 

}

void Renderer::generateColAdaptiveRenderTasks(
    const ScreenGrid& grid, const Camera* cam, World& w
) {

    // Retrieve the ray distances and resolution
    const std::vector<double>* pRayDistances = grid.getRayDistances();
    double dt = grid.getRayResolution();

    /* u, v are the coordinates of the pixel in the camera, ug and vg are the coordinates
     * of the pixel with respect to the grid size. */
    ui32_t u, v;

    ui32_t id;
    for (size_t ug = 0; ug < grid.width(); ug++) {
        
        ui32_t min_index = 0; 
        for (size_t vg = 1; vg < grid.height(); vg++) {

            size_t vp = vg - 1 + ug*grid.height();
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
            for (size_t vg = 0; vg < grid.height(); vg++) {
                id = grid.getGPixelId(ug, vg);
                cam->getPixelCoordinates(id, u, v);
                updateTaskQueue(TaskedPixel(id, u, v, dt));
            }

            releaseTaskQueue(cam, w);

        } else if (min_index == grid.height() - 1) {

            // Here we generate a single task starting from the bottom of the column
            for (int vg = grid.height() - 1; vg >= 0; vg--) {
                id = grid.getGPixelId(ug, vg); 
                cam->getPixelCoordinates(id, u, v);
                updateTaskQueue(TaskedPixel(id, u, v, dt));
            }

            releaseTaskQueue(cam, w);

        } else {

            /* Here we generate two tasks. The first one goes from the min element to the 
             * top of the column. The second one from the one after the min to the end 
             * of the column. */
            
            for (int vg = min_index; vg >= 0; vg--) {
                id = grid.getGPixelId(ug, vg); 
                cam->getPixelCoordinates(id, u, v);
                updateTaskQueue(TaskedPixel(id, u, v, dt)); 
            }

            releaseTaskQueue(cam, w);

            for (size_t vg = min_index + 1; vg < grid.height(); vg++) {
                id = grid.getGPixelId(ug, vg);
                cam->getPixelCoordinates(id, u, v); 
                updateTaskQueue(TaskedPixel(id, u, v, dt)); 
            }

            releaseTaskQueue(cam, w);
        }
    }

}

void Renderer::generateRowAdaptiveRenderTasks(
    const ScreenGrid& grid, const Camera* cam, World& w
) {

    // Retrieve the ray distances and resolution
    const std::vector<double>* pRayDistances = grid.getRayDistances();
    double dt = grid.getRayResolution(); 

    /* u, v are the coordinates of the pixel in the camera, ug and vg are the coordinates
     * of the pixel with respect to the grid size. */
    ui32_t u, v;

    ui32_t id;
    for (size_t vg = 0; vg < grid.height(); vg++) {
        
        ui32_t min_index = 0; 
        for (size_t ug = 1; ug < grid.width(); ug++) {

            size_t up = vg + (ug-1)*grid.height();
            size_t un = up + grid.height();

            // Update current minimum index value
            if ((pRayDistances->at(up) != inf) && 
                (pRayDistances->at(un) > pRayDistances->at(up))) {
                break;
            }

            min_index++;
        }
        
        if (min_index == 0) {
        
            // Here we generate a single task starting from the left of the row
            for (size_t ug = 0; ug < grid.width(); ug++) {
                
                // Retrieve the pixel ID and its coordinates
                id = grid.getGPixelId(ug, vg);
                cam->getPixelCoordinates(id, u, v);

                updateTaskQueue(TaskedPixel(id, u, v, dt));
            
            }

            releaseTaskQueue(cam, w);

        } else if (min_index == grid.width() - 1) {

            // Here we generate a single task starting from the right of the row
            for (int ug = grid.width() - 1; ug >= 0; ug--) {

                // Retrieve the pixel ID and its coordinates
                id = grid.getGPixelId(ug, vg); 
                cam->getPixelCoordinates(id, u, v);

                updateTaskQueue(TaskedPixel(id, u, v, dt));
            }

            releaseTaskQueue(cam, w);

        } else {

            /* Here we generate two tasks. The first one goes from the min element to the 
             * left of the row. The second one from the one after the min to the end 
             * of the row. */
            
            for (int ug = min_index; ug >= 0; ug--) {

                // Retrieve the pixel ID and coordinates
                id = grid.getGPixelId(ug, vg);
                cam->getPixelCoordinates(id, u, v); 

                updateTaskQueue(TaskedPixel(id, u, v, dt)); 
            }

            releaseTaskQueue(cam, w);

            for (size_t ug = min_index + 1; ug < grid.width(); ug++) {
                
                id = grid.getGPixelId(ug, vg);
                cam->getPixelCoordinates(id, u, v); 

                updateTaskQueue(TaskedPixel(id, u, v, dt)); 
            }

            releaseTaskQueue(cam, w);
        }
    }
}

double Renderer::initializeGrids(const Camera* cam, World& w) {

    // Compute the number of grids to be generated
    ui32_t ngw = (cam->width() + opts.gridWidth - 1) / opts.gridWidth;
    ui32_t ngh = (cam->height() + opts.gridHeight - 1) / opts.gridHeight; 
    
    // Update the total amount of grid cells
    ui32_t ng = ngw * ngh; 

    grids.clear();
    grids.reserve(ng);

    double res;
    double maxRes = -inf;

    for (size_t j = 0; j < ngh; j++) {
        for (size_t k = 0; k < ngw; k++) {

            // Compute the coordinates of the grid top-left pixel
            Pixel p0(k*opts.gridWidth, j*opts.gridHeight);

            // Initialize each screen grid
            ScreenGrid grid(p0, opts.gridWidth, opts.gridHeight, cam);

            // Compute the grid resolution 
            w.computeRayResolution(grid, cam);
            
            // Update the maximum ray resolution if different from infinity
            res = grid.getRayResolution();
            if (!std::isinf(res) && (res > maxRes)) {
                maxRes = res;
            }

            grids.push_back(grid);

        }
    }

    return maxRes;
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
    pixRes.reserve(nPixels);

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

void Renderer::renderBlack(const Camera* cam) {

    // Ray impact location is at an infinite distance.
    PixelData data; 
    data.t = inf;

    for (ui32_t id = 0; id < nPixels; id++) {

        // Update all pixels with the same value (and infinite resolution)
        RenderedPixel pix(id, 1, inf); 
        pix.addPixelData(data); 
        renderedPixels.push_back(pix); 

    }

    // Update the rendering status
    status = RenderingStatus::COMPLETED;

}

// This is the high-level function called by the user
void Renderer::render(const Camera* cam, World& w) {

    // Setup the render output variable.
    setupRenderer(cam, w); 

    // Generate the tasks and add them to the pool (i.e., the list of pixels to render)
    generateRenderTasks(cam, w); 
    
    /* If no intersection was found, the image is completely black and the rendering is 
     * deemed to be terminated. As such, we exit immediately. */
    if (status == RenderingStatus::COMPLETED) {
        return;
    }

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

void Renderer::computePixelBoundaries(const Camera* cam, ui32_t s) {
    
    // Clear the pixel boundaries and resolutions
    pixMinT.clear();
    pixMaxT.clear(); 
    pixRes.clear();

    int uMin, uMax; 
    int vMin, vMax; 

    ui32_t u, v; 
    size_t idx; 

    double t1, t2;
    double tMin, tMax;
    double dt, dtMin;

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

        tMin = inf; tMax = -inf; dtMin = inf;
        for (size_t j = uMin; j < uMax; j++) {   
            for (size_t k = vMin; k < vMax; k++) {

                // Retrieve new pixel id
                idx = cam->getPixelId(j, k);

                dt = renderedPixels[idx].pixResolution();
                t1 = renderedPixels[idx].pixMinDistance(); 
                t2 = renderedPixels[idx].pixMaxDistance();
                
                dtMin = dt < dtMin ? dt : dtMin;
                tMin = t1 < tMin ? t1 : tMin; 
                tMax = t2 > tMax ? t2 : tMax; 
            }
        }

        pixMinT.push_back(tMin); 
        pixMaxT.push_back(tMax); 
        pixRes.push_back(dtMin);

    }

}


ui32_t Renderer::generateAntiAliasingTasks(const Camera* cam, World& w) {

    // Retrieve current ray resolution.
    double rayRes;

    ui32_t u, v; 
    ui32_t nAliased = 0;

    for (size_t id = 0; id < nPixels; id++) {

        // Retrieve pixel coordinates
        cam->getPixelCoordinates(id, u, v); 

        // Retrieve the ray resolution for that pixel 
        rayRes = pixRes[id];

        if ((pixMaxT[id] - pixMinT[id]) >= opts.ssaa.threshold*rayRes) {

            // Generate pixel
            TaskedPixel tp(id, u, v, rayRes, opts.ssaa.nSamples);

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

    ui32_t u, v; 
    for (size_t id = 0; id < nPixels; id++) {
        
        // Retrieve pixel coordinates
        cam->getPixelCoordinates(id, u, v); 

        TaskedPixel tp(id, u, v, pixRes[id], 9); 

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