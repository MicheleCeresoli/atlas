
#include "arcadia.h"
#include "utils.h"

#include "opencv2/imgcodecs.hpp"

// Utility function to ensure images have the proper bits
void checkImageBits(int type) {
    if (type != CV_8UC1 && type != CV_16UC1) {
        throw std::invalid_argument("unsupported image bits precision.");
    }
}

void updateImageContent(cv::Mat& image, uint u, uint v, double c) {
    
    // Retrieve a pointer to the current pixel row and update the pixel value 
    if (image.type() == CV_8UC1) {
        uchar* pRow = image.ptr<uchar>(v); 
        pRow[u] = 255.999*c; 

    } else {
        uint16_t* pRow = image.ptr<uint16_t>(v); 
        pRow[u] = 65535.999*c;
    }
    
}


LunarRayTracer::LunarRayTracer(RayTracerOptions opts) : 
    world(opts.optsWorld, opts.nThreads), 
    renderer(opts.optsRenderer, opts.nThreads) {}


void LunarRayTracer::run() {

    // We clean-up the DOM at each run since the data that would be required for it 
    // is changed anyway.
    world.cleanupDOM();

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

bool LunarRayTracer::generateImageOptical(const std::string& filename, int type) {

    // Check bits
    checkImageBits(type); 

    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    // Create a grayscale image (8-bit single-channel)
    cv::Mat image(cam->height(), cam->width(), type, cv::Scalar(0));

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
        c /= (255*p.nSamples); 

        // Update the pixel content
        updateImageContent(image, u, v, c);

    }

    // Write the image
    return cv::imwrite(filename, image); 

}


bool LunarRayTracer::generateImageDEM(const std::string& filename, int type) {

    // Check bits
    checkImageBits(type);

    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    // Create a grayscale image (8-bit single-channel)
    cv::Mat image(cam->height(), cam->width(), type, cv::Scalar(0));

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

        // Update the pixel content
        updateImageContent(image, u, v, c);

    }

    // Write the image
    return cv::imwrite(filename, image); 

}


bool LunarRayTracer::generateDepthMap(const std::string& filename, int type) {

    // Check bits
    checkImageBits(type);

    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    // Create a grayscale image (8-bit single-channel)
    cv::Mat image(cam->height(), cam->width(), type, cv::Scalar(0));

    // First we parse all pixels to find the min and maximum distances
    double dMin = inf; 
    double dMax = -inf; 
    
    for (auto& p : *pixels) {

        if (p.pixMaxDistance() != inf)
            dMax = fmax(p.pixMaxDistance(), dMax); 

        if (p.pixMinDistance() != inf)
            dMin = fmin(p.pixMinDistance(), dMin);  

    }
    
    /* Since pixels have a minimum distance of 0, if dMin is still infinite it means 
     * that all pixels never crossed the Moon, i.e., the image is completely blank. */

    if (dMin == inf) {
        return cv::imwrite(filename, image); 
    }

    // Now we generate the normalised image with those values
    double dt = dMax - dMin; 
    
    double c; 
    uint u, v;

    for (auto& p : *pixels) {

        // Retrieve the pixel coordinates
        cam->getPixelCoordinates(p.id, u, v); 

        c = 0.0;
        for (size_t k = 0; k < p.nSamples; k++) {
            if (p.data[k].t != inf) {
                // Normalise and invert to have white as the closest distance.
                c += (dMax - p.data[k].t)/dt; 
            }
        }

        // Average the distance through all the samples
        c /= p.nSamples;

        // Update the pixel content
        updateImageContent(image, u, v, c);

    }

    // Write the image
    return cv::imwrite(filename, image); 
    
}



void LunarRayTracer::generateImageGCPs(const std::string& filename) {

}


void LunarRayTracer::getImageGCPs() {

}