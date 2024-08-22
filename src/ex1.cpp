
#include "vec3.h"
#include "utils.h"
#include "dcm.h"
#include "camera.h"
#include "renderer.h"
#include "world.h"

#include "pool.h"
#include "pixel.h"
#include "color.h"

#include <cmath>
#include <iostream>
#include <vector>

void toImage(Camera& cam, const std::vector<RenderedPixel> pixels)
{

    std::cout << "P3\n" << cam.width << ' ' << cam.height << "\n255\n"; 

    for (int j = 0; j < pixels.size(); j++)
    {
        write_color(std::cout, pixels[j].color);
    }

    std::clog << "\rDone. \n";

}


int main()
{
    int    cam_res = 640;
    double cam_fov = deg2rad(40); 

    double h = 1.5/std::sin(cam_fov/2);

    point3 cam_pos = point3(h, 0, 0); 
    dcm    cam_dcm = dcm(0, 0, -1, 0, 1, 0, 1, 0, 0);

    // Initialise the camera object
    Camera cam(cam_res, cam_fov);
    cam.set_dcm(cam_dcm); 
    cam.set_pos(cam_pos); 

    World w = World(); 

    // Create the Renderer (1 thread, batch size 64)
    Renderer renderer(1, 64);

    // Render the image
    std::vector<RenderedPixel> pixels = renderer.render(cam, w); 

    // Write the pixels to a PPM image file
    toImage(cam, pixels); 

    // ThreadPool pool(4); 

    // // Enqueue tasks for execution 
    // for (int j = 0; j < 7; j++)
    // {
    //     pool.addTask([j]() {
    //         std::cout << "Task " << j << " is running on thread " 
    //              << std::this_thread::get_id() << std::endl;

    //         // Simulate some work 
    //         std::this_thread::sleep_for(std::chrono::milliseconds(100));  
    //     });
    // }

    // std::cout << "Starting Jobs Queue" << std::endl; 
    // pool.start(); 

    // unsigned int nthreads = std::thread::hardware_concurrency(); 
    // std::cout << "Number of available threads: " << nthreads << std::endl; 

    return 0;
}