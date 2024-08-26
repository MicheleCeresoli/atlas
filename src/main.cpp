
#include "camera.h"
#include "dcm.h"
#include "pixel.h"
#include "raster.h"
#include "renderer.h"
#include "utils.h"
#include "vec2.h"
#include "vec3.h"
#include "world.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>


#define CAM_FOV (40) 

void writePixel(std::ostream& out, double x) {
    // Translate the [0,1] component values to the byte range [0,255]
    int rbyte = int(255.999*x);
    int gbyte = int(255.999*x);
    int bbyte = int(255.999*x);

    // Write out the pixel color components. 
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n'; 
}

void writeImage(const Camera& cam, const std::vector<double>& data) 
{   
    std::cout << "P3\n" << cam.width << ' ' << cam.height << "\n255\n"; 
    for (int j = 0; j < data.size(); j++) {
        writePixel(std::cout, data[j]); 
    }

}

void makeImageDEM(const Camera& cam, const std::vector<RenderedPixel>& pixels)
{
    // First, we retrieve the minimum and maximum values of the data 
    auto minmax = std::minmax_element(
        pixels.begin(), pixels.end(),
        [](const RenderedPixel& p1, const RenderedPixel& p2) {
            return p1.d.s[0] < p2.d.s[0]; 
        }
    );

    std::clog << "The min height is: " << (*minmax.first).d.s[0] << std::endl; 
    std::clog << "The max height is: " << (*minmax.second).d.s[0] << std::endl; 

    double min = minmax.first->d.s[0]; 
    double max = minmax.second->d.s[0]; 

    std::vector<double> imgData; 
    imgData.reserve(pixels.size());

    double t; 
    for (auto p : pixels) {
        t = (p.d.s[0] - min)/(max - min);  
        imgData.push_back(t); 
    }

    writeImage(cam, imgData); 

}

void makeImageLIDAR(const Camera& cam, const std::vector<RenderedPixel>& pixels)
{

    // First, we retrieve the minimum and maximum values of the data 
    auto minmax = std::minmax_element(
        pixels.begin(), pixels.end(),
        [](const RenderedPixel& p1, const RenderedPixel& p2) {
            return p1.d.t < p2.d.t; 
        }
    );

    std::clog << "The minimum altitude is: " << (*minmax.first).d.t << std::endl; 
    std::clog << "The maximum altitude is: " << (*minmax.second).d.t << std::endl; 

    double min = minmax.first->d.t; 
    double max = minmax.second->d.t; 

    std::vector<double> imgData; 
    imgData.reserve(pixels.size());

    double t; 
    for (auto p : pixels) {
        t = (p.d.t - min)/(max - min);  
        imgData.push_back(t); 
    }

    writeImage(cam, imgData); 

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

    double h = 40e3 + 1737400;
    double lon = deg2rad(-165);
    double lat = deg2rad(62);

    point3 cam_pos = sph2car(point3(h, lon, lat));
    std::clog << std::fixed << std::setprecision(3) << std::endl;

    // point3 cam_pos = point3(h, 0, 0); 
    // dcm    cam_dcm = dcm(0, 0, -1, 0, 1, 0, 1, 0, 0);
    
    dcm cam_dcm = pos2dcm(cam_pos); 
    std::clog << "Position: " << std::endl << cam_pos << "\n\n"; 
    std::clog << "DCM: "  << std::endl << cam_dcm << std::endl; 

    // Initialise the camera object
    Camera cam(cam_res, cam_fov);
    cam.set_dcm(cam_dcm); 
    cam.set_pos(cam_pos); 

    int nThreads = 7;   

    std::string filename = "../resources/CE2_GRAS_DEM_50m_C001_63N165W_A.tif";
    RasterFile raster(filename, nThreads); 
    raster.loadBands(); 

    World w(raster); 

    // Create the Renderer (1 thread, batch size 64)
    Renderer renderer(nThreads, 640);

    // Render the image
    std::vector<RenderedPixel> pixels = renderer.render(cam, w); 

    // Write the pixels to a PPM image file
    // makeImageLIDAR(cam, pixels); 
    makeImageDEM(cam, pixels); 

    return 0;
}