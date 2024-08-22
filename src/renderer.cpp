
#include "renderer.h"

#include <cmath>
#include <mutex>

// Constructor
Renderer::Renderer(size_t nThreads, size_t batch_size) : 
    pool(ThreadPool(nThreads)), batch_size(batch_size) {}



// This function stores the output of each render task in the original class
void Renderer::saveRenderTaskOutput(const std::vector<RenderedPixel> pixels)
{
    {
        std::unique_lock<std::mutex> lock(renderMutex); 
        renderedPixels.push_back(pixels); 
    }
}

// This function renders a batch of pixels
void Renderer::renderTask(Camera& cam, World& w, const std::vector<Pixel> &pixels) {

    // Create a vector storing the pixels to be rendered with the given memory
    std::vector<RenderedPixel> output;
    output.reserve(pixels.size()); 

    RenderedPixel rPix; 

    for (int j = 0; j < pixels.size(); j++)
    {
        Pixel pix = pixels[j]; 
        rPix = {.pixel = pix}; 
        
        // Retrieve camera ray for this pixel
        Ray ray = cam.get_ray(pix.u, pix.v); 

        // Compute pixel color
        if (w.trace_ray(ray)) 
        {
            rPix.color = Color(1,0,0);
        } 
        else 
        {
            rPix.color = Color(); 
        }

        // Add the pixel to the list of computed pixels
        output.push_back(rPix); 

    }
    
    // Save the rendered pixels in the Rendeder class 
    saveRenderTaskOutput(output); 

}

// This function generates all the tasks required to render an image.
void Renderer::generateRenderTasks(Camera& cam) {
    
    // List of pixels for each task
    std::vector<Pixel> task; 
    Pixel pix; 

    // This should not be necessary...
    renderTasks.clear();

    // Compute the total number of pixels that must be rendered. 
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
            renderTasks.push_back(task); 
            task.clear(); 
        }

        i++; 
    }

}

// This function post-processes the outputs of all tasks to generated an 
// orderered list of pixels.
std::vector<RenderedPixel> Renderer::processRenderOutput(Camera& cam)
{
    // TODO: this function will need to be changed accordingly to how 
    // tasks are generated 

    // Compute total number of pixels 
    int nPixels = cam.width * cam.height;

    // Create the output vector to match the expected number of pixels.
    std::vector<RenderedPixel> output; 
    output.reserve(nPixels);

    for (int j = 0; j < renderedPixels.size(); j++)
    {
        for (int i = 0; i < renderedPixels[j].size(); i++)
        {
            output.push_back(renderedPixels[j][i]);
        }
    }

    renderedPixels.clear(); 
    return output;
}


// This is the high-level function called by the user
std::vector<RenderedPixel> Renderer::render(Camera& cam, World& w) {

    // Generate the list of pixels to be rendered; 
    generateRenderTasks(cam); 

    // Process each rendering task sequentially (for now)
    for (int j = 0; j < renderTasks.size(); j++)
    {
        renderTask(cam, w, renderTasks[j]);
    }

    // At this point we need to re-order all the pixels in the previous
    return processRenderOutput(cam);

}

