
#include "renderer.h"
#include <cmath>

Renderer::Renderer(size_t nThreads, size_t batch_size) : 
    pool(ThreadPool(nThreads)), batch_size(batch_size) {}


// This is the high-level function called by the user
std::vector<RenderedPixel> Renderer::render(Camera& cam, World& w) {

    // Temporary batch size to determine the maximum number of pixels in each 
    // render task.
    size_t batch_size = 64; 

    // Compute the total number of pixels that must be rendered. 
    // int nPixels = cam.width * cam.height; 
    // int nTasks = (int) ceil((double)nPixels / batch_size); 

    // List of tasks
    std::vector<std::vector<Pixel>> tasks; 
    // List of pixels of each task
    std::vector<Pixel> task_j; 

    // Assign all the pixels to a specific rendering task.
    int j = 0, i = 0;
    while (j < cam.height)
    {
        if (i >= cam.width)
        {
            j++; 
            i = 0; 
        } 

        Pixel pix = {.u = i, .v = j};
        task_j.push_back(pix);  

        if (task_j.size() >= batch_size)
        {
            tasks.push_back(task_j); 
            task_j.clear(); 
        }

        i++; 
    }

    // Process each rendering task sequentially (for now)
    std::vector<std::vector<RenderedPixel>> rendered;
    std::vector<RenderedPixel> taskRender;

    for (int j = 0; j < tasks.size(); j++)
    {
        taskRender = renderTask(cam, w, tasks[j]);
        rendered.push_back(taskRender);
    }

    // At this point we need to re-order all the pixels in the previous
    std::vector<RenderedPixel> output; 

    for (int j = 0; j < rendered.size(); j++)
    {
        for (int i = 0; i < rendered[j].size(); i++)
        {
            output.push_back(rendered[j][i]);
        }
    }

    return output;

}


std::vector<RenderedPixel> 
Renderer::renderTask(Camera& cam, World& w, const std::vector<Pixel> pixels) {

    std::vector<RenderedPixel> rendered;
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
        rendered.push_back(rPix); 

    }
    
    return rendered; 

}