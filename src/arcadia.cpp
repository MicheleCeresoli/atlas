
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

    // Convert camera position to spherical coordinates
    point3 sph = car2sph(cam->getPos()); 

    // Convert geographic coordinates to degrees
    point2 s2 = rad2deg(point2(sph[1], sph[2]));

    // Retrieve the exact altitude from the DEM
    altitude = world.sampleDEM(s2);

}


// Image Generation Routines 

bool LunarRayTracer::generateImageOptical(const std::string& filename) {

    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    // Create a grayscale image (8-bit single-channel)
    cv::Mat image(cam->height(), cam->width(), CV_8UC1, cv::Scalar(0));

    double c; 
    point2 s; 

    uint u, v;
    uchar* pRow;
     
    for (auto& p : *pixels) {
        
        // Retrieve pixel coordinates on the image
        cam->getPixelCoordinates(p.id, u, v); 

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


bool LunarRayTracer::generateImageDEM(const std::string& filename) {

    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    // Create a grayscale image (8-bit single-channel)
    cv::Mat image(cam->height(), cam->width(), CV_8UC1, cv::Scalar(0));

    double c; 

    uint u, v;
    uchar* pRow;

    // Retrieve the DEM maximum and minimum radii 
    double minR = world.minRadius();
    double maxR = world.maxRadius(); 
    double dR = maxR - minR; 

    for (auto& p : *pixels) {
        
        // Retrieve pixel coordinates on the image
        cam->getPixelCoordinates(p.id, u, v); 

        c = 0.0;
        for (size_t k = 0; k < p.nSamples; k++) {

            if (p.data[k].t != inf) {
                // Retrieve point distance from center and normalise 
                c += (p.data[k].s[0] - minR)/dR;
            }
        }

        // Average the pixel color through all the samples.
        c /= p.nSamples; 

        // Retrieve a pointer to the current pixel row and update the pixel value 
        pRow = image.ptr<uchar>(v);
        pRow[u] = uint(255.999*c); 

    }

    // Write the image
    return cv::imwrite(filename, image); 

}


void LunarRayTracer::generateImageGCPs(const std::string& filename) {

}


void LunarRayTracer::getImageGCPs() {

}