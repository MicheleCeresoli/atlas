
#include "arcadia.h"
#include "utils.h"

#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"


LunarRayTracer::LunarRayTracer(RayTracerOptions opts) : 
    world(opts.optsWorld, opts.nThreads), 
    renderer(opts.optsRenderer, opts.nThreads) {}


void LunarRayTracer::run() {

    // Compute the ray rendering resolution.
    world.computeRayResolution(cam); 

    // Ray trace all the pixels in the camera
    renderer.render(cam, world);    

    // Compute the Camera altitude wrt the DEM surface
    sampleAltitude();
    
    // Unloads unused DEM files data from memory.
    world.cleanupDEM();

}


// Settings Retrieval
void LunarRayTracer::sampleAltitude() {

    // Create a ray centered at the camera position in the Nadir direction
    Ray nadir(cam.get_pos(), -cam.get_pos());

    double t[2];

    // Update the altitude 
    altitude = world.traceRay(nadir, t, 0).t;

}


// Image Generation Routines 

bool LunarRayTracer::generateImageOptical(const std::string& filename) {

    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    // Create a grayscale image (8-bit single-channel)
    cv::Mat image(cam.height(), cam.width(), CV_8UC1, cv::Scalar(0));

    double c; 
    point2 s; 

    uint u, v;
    uchar* pRow;
     
    for (auto& p : *pixels) {
        
        // Retrieve pixel coordinates on the image
        cam.pixel_coord(p.id, u, v); 

        c = 0.0;
        for (size_t k = 0; k < p.nSamples; k++) {

            if (p.data[k].t != inf) {
                
                // Retrieve sample longitude and latitudes in degrees
                s[0] = rad2deg(p.data[k].s[1]); 
                s[1] = rad2deg(p.data[k].s[2]); 
                
                // Sample the DOM at that location
                c += world.sampleDOM(s);
            }
        }

        // Average the pixel color through all the samples.
        c /= p.nSamples; 

        // Retrieve a pointer to the current pixel row and update the pixel value 
        pRow = image.ptr<uchar>(v);
        pRow[u] = c; 

    }

    // Write the image
    return cv::imwrite(filename, image); 

}


void LunarRayTracer::generateImageLIDAR(const std::string& filename) {

}


void LunarRayTracer::generateImageDEM(const std::string& filename) {

}


void LunarRayTracer::generateImageGCPs(const std::string& filename) {

}


void LunarRayTracer::getImageGCPs() {

}