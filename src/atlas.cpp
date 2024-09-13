
#include "atlas.h"
#include "utils.h"

#include <filesystem>

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
        pRow[u] = static_cast<uint8_t>(255.999*c); 

    } else {
        uint16_t* pRow = image.ptr<uint16_t>(v); 
        pRow[u] = static_cast<uint16_t>(65535.999*c);
    }
    
}


LunarRayTracer::LunarRayTracer(RayTracerOptions opts) : 
    world(opts.optsWorld, opts.nThreads), 
    renderer(opts.optsRenderer, opts.nThreads) {}


void LunarRayTracer::run() {

    // Check a CAM has been assigned 
    checkCamPointer(); 

    // Unloads unused DEM and DOM files data from memory.
    world.cleanup();

    // Compute the ray rendering resolution.
    world.computeRayResolution(cam); 

    // Ray trace all the pixels in the camera
    renderer.render(cam, world);    
    
}


// Settings Retrieval
double LunarRayTracer::getAltitude() {

    // Check a CAM has been assigned 
    checkCamPointer(); 
    
    // Convert camera position to spherical coordinates
    point3 sph = car2sph(cam->getPos()); 

    // Convert geographic coordinates to degrees
    point2 s2 = rad2deg(point2(sph[1], sph[2]));

    // Retrieve the exact altitude from the DEM
    return sph[0] - world.sampleDEM(s2);
    
}


// Image Generation Routines 

bool LunarRayTracer::generateImageOptical(const std::string& filename, int type) {

    // Check bits
    checkImageBits(type); 
    // Check camera pointer
    checkCamPointer();
    // Check rendering status 
    checkRenderStatus();

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
    bool flag = cv::imwrite(filename, image); 

    if (!flag) {
        throw std::runtime_error("failed to generate optical image.");
    }

    if (logLevel >= LogLevel::MINIMAL) {
        displayTime();
        std::clog << "Generated: " << "\033[32m" << filename << "\033[0m" << std::endl;
    }

    return flag;

}


bool LunarRayTracer::generateImageDEM(const std::string& filename, int type) {

    // Check bits
    checkImageBits(type);
    // Check camera pointer
    checkCamPointer();
    // Check rendering status 
    checkRenderStatus();

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
    bool flag = cv::imwrite(filename, image); 

    if (!flag) {
        throw std::runtime_error("failed to generate DEM image.");
    }

    if (logLevel >= LogLevel::MINIMAL) {
        displayTime();
        std::clog << "Generated: " << "\033[32m" << filename << "\033[0m" << std::endl;
    }

    return flag;

}


bool LunarRayTracer::generateDepthMap(const std::string& filename, int type) {

    // Check bits
    checkImageBits(type);
    // Check camera pointer
    checkCamPointer();
    // Check rendering status 
    checkRenderStatus();

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
    bool flag = cv::imwrite(filename, image); 

    if (!flag) {
        throw std::runtime_error("failed to generate depth map.");
    }

    if (logLevel >= LogLevel::MINIMAL) {
        displayTime();
        std::clog << "Generated: " << "\033[32m" << filename << "\033[0m" << std::endl;
    }

    return flag;
    
}


void LunarRayTracer::generateGCPs(const std::string& filename, int stride) {

    // Check camera pointer
    checkCamPointer();
    // Check rendering status 
    checkRenderStatus();

    // Generate filepath object 
    auto filepath = std::filesystem::path(filename); 

    // Set the file delimiter depending on its format
    std::string dl; 
    if (filepath.extension() == ".txt") {
        dl = " ";

    } else if (filepath.extension() == ".csv") {
        dl = ", ";

    } else {
        throw std::invalid_argument(
            "GCP data can only be exported to TXT or CSV file formats."
        );
    }
    
    // Try to create the file
    std::ofstream file(filename); 
    if (!file.is_open()) {
        throw std::runtime_error("unable to create the file in this path.");
    }
    
    // Retrieve the pixel data
    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    uint id; 

    double lon, lat; 
    PixelData data;

    for (size_t u = 0; u < cam->width(); u += stride) {
        for (size_t v = 0; v < cam->height(); v += stride) {

            // Retrieve pixel id 
            id = cam->getPixelId(u, v);

            // Retrieve data relative to the pixel center
            data = (*pixels)[id].data[0]; 
            if (data.t != inf) {
            
                // Retrieve longitude and latitude in degrees
                lon = rad2deg(data.s[1]);
                lat = rad2deg(data.s[2]);

                // Write the data to the CSV file
                file << u << dl << v << dl << lon << dl << lat << "\n"; 
            
            }
        }
    }


    // Close the file
    file.close(); 

    if (logLevel >= LogLevel::MINIMAL) {
        displayTime();
        std::clog << "Exported GPCs to: " << "\033[32m" 
                  << filename << "\033[0m" << std::endl;
    }

}


void LunarRayTracer::exportRayTracedInfo(const std::string& filename) {

    // Check camera pointer
    checkCamPointer();
    // Check rendering status 
    checkRenderStatus();

    // Try to create the file
    std::ofstream file(filename + ".brd", std::ios::binary); 
    if (!file.is_open()) {
        throw std::runtime_error("unable to create the file in this path.");
    }

    uint camWidth = cam->width(); 
    uint camHeight = cam->height();

    // Write the camera resolution 
    file.write(reinterpret_cast<const char*>(&camWidth), sizeof(camWidth));
    file.write(reinterpret_cast<const char*>(&camHeight), sizeof(camHeight));

    vec3 camPos = cam->getPos(); 
    dcm  camDCM = cam->getDCM();

    // Write camera position and orientation
    file.write(reinterpret_cast<const char*>(&camPos), sizeof(camPos));
    file.write(reinterpret_cast<const char*>(&camDCM), sizeof(camDCM));

    const std::vector<RenderedPixel>* pixels = renderer.getRenderedPixels();

    size_t nPix = pixels->size();
    
    // Write the number of rendered pixels that are going to be written 
    file.write(reinterpret_cast<const char*>(&nPix), sizeof(nPix));

    // Write rendered pixels data
    for (size_t k = 0; k < nPix; k++) {
        
        RenderedPixel p = (*pixels)[k];

        // Write the ID 
        file.write(reinterpret_cast<const char*>(&p.id), sizeof(p.id));
        // Write the number of samples
        file.write(reinterpret_cast<const char*>(&p.nSamples), sizeof(p.nSamples));

        for (size_t j = 0; j < p.nSamples; j++) {
            file.write(reinterpret_cast<const char*>(&p.data[j]), sizeof(p.data[j]));
        }

    }

    // Close the file 
    file.close();

    if (logLevel >= LogLevel::MINIMAL) {
        displayTime();
        std::clog << "Exported ray-traced data to: " << "\033[31m" 
                  << filename + ".brd" << "\033[0m" << std::endl;
    }

}

void LunarRayTracer::importRayTracedInfo(const std::string& filename) {

    // Check camera pointer
    checkCamPointer();

    std::ifstream file(filename + ".brd", std::ios::binary); 
    if (!file.is_open()){
        throw std::runtime_error("unable to open the file in this path.");
    }

    // Retrieve camera width and height 
    uint camWidth, camHeight; 
    file.read(reinterpret_cast<char*>(&camWidth), sizeof(camWidth));
    file.read(reinterpret_cast<char*>(&camHeight), sizeof(camHeight));

    if ((camHeight != cam->height()) || (camWidth != cam->width())) {
        throw std::runtime_error("incompatible camera dimensions found.");
    }

    vec3 camPos;                      
    dcm camDCM;

    // Read the camera position and orientation
    file.read(reinterpret_cast<char*>(&camPos), sizeof(camPos));
    file.read(reinterpret_cast<char*>(&camDCM), sizeof(camDCM));

    // Update the camera 
    cam->setPos(camPos); 
    cam->setDCM(camDCM); 

    // Retrieve the number of pixels to be read
    size_t nPix; 
    file.read(reinterpret_cast<char*>(&nPix), sizeof(nPix)); 

    std::vector<RenderedPixel> pixels; 
    pixels.reserve(nPix); 

    uint id;         
    size_t nSamples; 
    PixelData data;

    for (size_t k = 0; k < nPix; k++) {

        // Retrieve pixel ID and number of samples
        file.read(reinterpret_cast<char*>(&id), sizeof(id)); 
        file.read(reinterpret_cast<char*>(&nSamples), sizeof(nSamples));

        // Retrieve the data of each sample
        RenderedPixel pk(id, nSamples);

        for (size_t j = 0; j < nSamples; j++) {
            file.read(reinterpret_cast<char*>(&data), sizeof(data));
            pk.addPixelData(data); 
        }

        // Append the rendered pixel
        pixels.push_back(pk);

    }

    // Update the renderer status with this pixels
    renderer.importRenderedData(pixels);

    // Close the file 
    file.close();

    if (logLevel >= LogLevel::MINIMAL) {
        displayTime();
        std::clog << "Imported ray-traced data from: " << "\033[31m" 
                  << filename + ".brd" << "\033[0m" << std::endl;
    }

}

void LunarRayTracer::checkCamPointer() {
    if (cam == NULL) {
        throw std::runtime_error("missing camera information.");
    }
}

void LunarRayTracer::checkRenderStatus() {
    if (renderer.getStatus() != RenderingStatus::COMPLETED) {
        throw std::runtime_error("missing ray-tracing information.");
    }
}
