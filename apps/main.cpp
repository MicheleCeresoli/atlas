
#include "camera.h"
#include "dcm.h"
#include "dem.h"
#include "pixel.h"
#include "raster.h"
#include "renderer.h"
#include "utils.h"
#include "vec2.h"
#include "vec3.h"
#include "world.h"

#include "arcadia.h"

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
    std::cout << "P3\n" << cam.width() << ' ' << cam.height() << "\n255\n"; 
    for (int j = 0; j < data.size(); j++) {
        writePixel(std::cout, data[j]); 
    }

}

void makeImageDEM(const Camera& cam, const std::vector<RenderedPixel>& pixels)
{
    /* First, we retrieve the minimum and maximum values of the data. We do it 
       manually because we have to exclude pixels that have invalid data. */    

    double min = inf, max = -inf; 
    double r;

    for (size_t j = 0; j < pixels.size(); j++) {
        for (size_t k = 0; k < pixels[j].nSamples; k++) {
            
            r = pixels[j].data[k].s[0]; 
            if (r > max) {
                max = r; 
            } else if ((r < min) && (r != 0.0)) {
                min = r;
            } 
        }
    }

    // std::clog << "The min height is: " << min << std::endl; 
    // std::clog << "The max height is: " << max << std::endl; 

    std::vector<double> imgData; 
    imgData.reserve(pixels.size());

    double t; 
    for (auto p : pixels) {

        t = 0.0;
        for (size_t k = 0; k < p.nSamples; k++) {
            if (p.data[k].t != inf) {
                t += (p.data[k].s[0] - min)/(max - min);
            }
        }

        t /= (double)p.nSamples;
        imgData.push_back(t); 

        // imgData.push_back((p.nSamples > 1) ? 1 : 0);

    }

    writeImage(cam, imgData); 

}

void makeImageLIDAR(const Camera& cam, const std::vector<RenderedPixel>& pixels, bool invert)
{

    /* First, we retrieve the minimum and maximum values of the data. We do it 
       manually because we have to exclude pixels that have invalid data. */    

    double min = inf, max = -inf; 
    double t;

    for (size_t j = 0; j < pixels.size(); j++) {
        
        t = pixels[j].pixMinDistance(); 
        if (t < min) 
            min = t; 

        t = pixels[j].pixMaxDistance(); 
        if ((t > max) && (t != inf))
            max = t; 

    }

    // std::clog << "The min distance is: " << min << std::endl; 
    // std::clog << "The max distance is: " << max << std::endl; 

    std::vector<double> imgData; 
    imgData.reserve(pixels.size());

    for (auto p : pixels) {

        t = 0.0; 
        for (size_t k = 0; k < p.nSamples; k++) {
            if (p.data[0].t != inf) {
                t += (p.data[0].t - min)/(max - min);  
            }
        }
        
        t /= (double)p.nSamples; 
        t = invert ? 1.0 - t : t; 

        imgData.push_back(t); 

    }

    writeImage(cam, imgData); 

}

void makeImageOptical(const Camera& cam, DOM& dom, const std::vector<RenderedPixel>& pixels)
{
  /* First, we retrieve the minimum and maximum values of the data. We do it 
       manually because we have to exclude pixels that have invalid data. */    

    // std::clog << "The min distance is: " << min << std::endl; 
    // std::clog << "The max distance is: " << max << std::endl; 

    std::vector<double> imgData; 
    imgData.reserve(pixels.size());

    double c; 
    point2 s; 

    for (auto p : pixels) {

        s = rad2deg(point2(p.data[0].s[1], p.data[0].s[2]));
        // std::clog << s << std::endl; 

        // c = dom.getColor(s, true, 0)/255;

        for (size_t k = 0; k < p.nSamples; k++) {
            if (p.data[k].t != inf) {
                
                s = rad2deg(point2(p.data[k].s[1], p.data[k].s[2]));
                c += dom.getColor(s, true, 0);   
            }
        }

        c /= (255*p.nSamples);
        
        // t /= (double)p.nSamples; 
        // t = invert ? 1.0 - t : t; 

        imgData.push_back(c); 

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

    std::clog << std::endl;

    uint   cam_res = 640;
    double cam_fov = deg2rad(CAM_FOV); 

    // double h = 1.1*1738e3/std::sin(cam_fov/2);

    double h = 350e3 + 1737400;
    double lon = deg2rad(-165);
    double lat = deg2rad(62);

    point3 cam_pos = sph2car(point3(h, lon, lat));

    dcm A_cam2lvlh = angle2dcm("Y", deg2rad(60)).transpose();
    dcm A_lvlh2in  = pos2dcm(cam_pos);
    
    dcm A_cam2in = A_lvlh2in*A_cam2lvlh;

    std::clog << "Position: " << std::endl << cam_pos << "\n\n"; 
    std::clog << "DCM: "  << std::endl << A_cam2in << std::endl; 

    // // Initialise the camera object
    Camera cam(cam_res, cam_fov);
    cam.set_dcm(A_cam2in); 
    cam.set_pos(cam_pos); 
   

    std::vector<std::string> demFiles;

    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_A001_87N000W_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_B001_77N158W_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_B002_77N113W_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_B008_77N158E_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_C001_63N165W_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_C002_63N135W_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_C012_63N165E_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_D001_49N168W_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_D002_49N144W_A.tif");
    demFiles.push_back("../resources/dem/CE2_GRAS_DEM_50m_D015_49N168E_A.tif");


    std::vector<std::string> domFiles; 
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_A001_87N000W_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_B001_77N158W_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_B002_77N113W_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_B008_77N158E_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_C001_63N165W_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_C002_63N135W_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_C012_63N165E_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_D001_49N168W_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_D002_49N144W_A.tif");
    domFiles.push_back("../resources/dom/CE2_GRAS_DOM_50m_D015_49N168E_A.tif");


    RenderingOptions optsRenderer = {
        .ssaa = SSAAOptions{.nSamples = 4, .active = true, .threshold = 0}, 
        .batchSize = 640,
        .displayInfo = true
    };


    WorldOptions optsWorld(demFiles, domFiles); 
    optsWorld.displayInfo = true; 

    size_t nThreads = 6;
    
    RayTracerOptions opts {
        .nThreads = nThreads, 
        .optsWorld = optsWorld, 
        .optsRenderer = optsRenderer
    };
    

    LunarRayTracer tracer = LunarRayTracer(opts);

    tracer.updateCamera(cam); 
    tracer.run(); 

    tracer.generateImageOptical("testImage5.png");

    // Write the pixels to a PPM image file
    // makeImageLIDAR(cam, pixels, true); 
    // makeImageDEM(cam, pixels); 
    // makeImageOptical(cam, w.dom, pixels);

    std::clog << std::endl; 
    return 0;
}


/*

    RasterFile raster(filename, nThreads); 
    raster.loadBands(); 

    double lon[4] = {-180, -180, -150, -150}; 
    double lat[4] = {56, 70, 56, 70}; 

    std::cout << "width: " << raster.width() << ", height: " << raster.height() << std::endl; 

    for (int j = 0; j < 4; j++) {
        
        point2 p(lon[j], lat[j]); 
        point2 m = raster.sph2pix(p, 0); 

        std::cout << "Lon: " << lon[j] << ", Lat: " << lat[j];
        std::cout << ", Pixel (" << m[0] << ", " << m[1] << ")";

        point2 ip = point2(double(int(m[0])), double(int(m[1]))); 
        std::cout << ", Value: " << raster.getBandData(ip[0], ip[1]) << std::endl;
        
    }

    double lon_bounds[2]; 
    double lat_bounds[2];

    computeRasterBounds(filename, lon_bounds, lat_bounds); 
    std::cout << "Lon: (" << lon_bounds[0] << ", " << lon_bounds[1] << ")" << std::endl;
    std::cout << "Lat: (" << lat_bounds[0] << ", " << lat_bounds[1] << ")" << std::endl;

    point2 p(lon, lat); 
    std::cout << raster.height() << "x" << raster.width() << std::endl; 

    std::cout << raster.sph2pix(p, 0) << std::endl;
    
    DEM dem(filename, nThreads);

    std::cout << "Mean radius: " << dem.getMeanRadius() << std::endl; 

    std::cout << "Min altitude: " << dem.getMinAltitude() << std::endl; 
    std::cout << "Max altitude: " << dem.getMaxAltitude() << std::endl; 

    RasterFile raster(filename, nThreads); 
    raster.loadBands(); 

    // std::cout << angle2dcm("X", deg2rad(10)) << std::endl; 
    // std::cout << angle2dcm("Y", deg2rad(10)) << std::endl; 
    // std::cout << angle2dcm("Z", deg2rad(10)) << std::endl; 

    // std::cout << angle2dcm("XY", deg2rad(10), deg2rad(70)) << std::endl; 
    // std::cout << angle2dcm("XZ", deg2rad(10), deg2rad(70)) << std::endl; 
    // std::cout << angle2dcm("YX", deg2rad(10), deg2rad(70)) << std::endl; 
    // std::cout << angle2dcm("YZ", deg2rad(10), deg2rad(70)) << std::endl; 
    // std::cout << angle2dcm("ZX", deg2rad(10), deg2rad(70)) << std::endl; 
    // std::cout << angle2dcm("ZY", deg2rad(10), deg2rad(70)) << std::endl; 

    // std::cout << angle2dcm("ZYX", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("XYX", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("XYZ", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("XZX", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("XZY", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("YXY", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("YXZ", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("YZX", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("YZY", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("ZXY", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("ZXZ", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 
    // std::cout << angle2dcm("ZYZ", deg2rad(10), deg2rad(70), deg2rad(25)) << std::endl; 


*/