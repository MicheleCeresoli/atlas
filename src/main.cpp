

#include "affine.h"
#include "vec2.h"
#include "vec3.h"
#include "utils.h"
#include "dcm.h"
#include "camera.h"
#include "renderer.h"
#include "world.h"

#include "pool.h"
#include "pixel.h"
#include "raster.h"
#include "crsutils.h"

#include "gdal_priv.h"

#include <cmath>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <thread>

#include <fstream>

#define CAM_FOV (40) 

void write_pixel(std::ostream& out, const PixelData& data) {
    
    auto g = 0.0;
    auto b = 0.0;

    auto r = data.t;
    if (r == inf) {
        r = 0.0;
    } else {
        r /= 10e3;
        g = r; 
        b = r; 
    }

    // Translate the [0,1] component values to the byte range [0,255]
    int rbyte = int(255.999*r);
    int gbyte = int(255.999*g);
    int bbyte = int(255.999*b);

    // Write out the pixel color components. 
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n'; 

}

void toImage(Camera& cam, const std::vector<RenderedPixel> pixels)
{

    std::cout << "P3\n" << cam.width << ' ' << cam.height << "\n255\n"; 

    for (int j = 0; j < pixels.size(); j++)
    {
        write_pixel(std::cout, pixels[j].d);
    }

    std::clog << "\rDone. \n";

}

    
dcm pos2dcm(point3 pos)
{
    vec3 uz = -unit_vector(pos);
    
    vec3 k(0.0, 0.0, 1.0); 
    vec3 uy = unit_vector(cross(uz, k)); 
    
    vec3 ux = unit_vector(cross(uy, uz)); 

    return dcm(
        ux[0], uy[0], uz[0], 
        ux[1], uy[1], uz[1], 
        ux[2], uy[2], uz[2]);

}

int main(int argc, const char* argv[])
{

    GDALAllRegister();

    int    cam_res = 640;
    double cam_fov = deg2rad(CAM_FOV); 

    // double h = 1.1*1738e3/std::sin(cam_fov/2);

    double h = 10e3 + 1737400;
    double lon = deg2rad(-165);
    double lat = deg2rad(62);

    point3 cam_pos = sph2car(point3(h, lon, lat));
    std::clog << std::fixed << std::setprecision(3) << std::endl;

    // point3 cam_pos = point3(h, 0, 0); 
    // dcm    cam_dcm = dcm(0, 0, -1, 0, 1, 0, 1, 0, 0);
    
    dcm cam_dcm = pos2dcm(cam_pos); 
    std::clog << cam_pos << std::endl;
    std::clog << cam_dcm << std::endl;

    // Initialise the camera object
    Camera cam(cam_res, cam_fov);
    cam.set_dcm(cam_dcm); 
    cam.set_pos(cam_pos); 

    int nThreads = 7;   

    std::string filename = "../CE2_GRAS_DEM_50m_C001_63N165W_A.tif";
    RasterFile raster(filename, nThreads); 
    raster.loadBands(); 

    World w(raster); 

    // Create the Renderer (1 thread, batch size 64)
    Renderer renderer(nThreads, 640);

    // Render the image
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<RenderedPixel> pixels = renderer.render(cam, w); 
    auto t2 = std::chrono::high_resolution_clock::now();

    // FIXME: pixel color is divided by a fucking great distance

    // Write the pixels to a PPM image file
    toImage(cam, pixels); 

    return 0;
}