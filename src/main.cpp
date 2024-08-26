
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

void computeRasterBounds(const std::string& filename, double* lon_bounds, double* lat_bounds) {

    // This routine is super specific for Chang'e DEM files! 

    std::size_t len = filename.size(); 

    int lat_id = filename[len - 18UL];
    double dlon = 360.0; 

    if (lat_id == 65) {
        lat_bounds[0] = 84.0; 
        lat_bounds[1] = 90.0;
    } 
    else if (lat_id == 78) {
        lat_bounds[0] = -90; 
        lat_bounds[1] = -84;
    } 
    else {
        
        double dlat = (lat_id - 66)*14; 
        lat_bounds[0] = 70 - dlat;
        lat_bounds[1] = 84.0 - dlat;

        // Compute the longitude span of that tile 
        switch (lat_id) {

            case 66:
            case 77: 
                dlon = 45.0;
                break; 

            case 67: 
            case 76: 
                dlon = 30.0;
                break; 

            case 68: 
            case 75:
                dlon = 24;
                break; 

            case 69: 
            case 74: 
                dlon = 20.0;
                break;
            
            default: 
                dlon  = 18.0;
                break;

        }
        
    }

    int lon_id = (int)atof(filename.substr(len - 17UL, 3).c_str()); 
 
    lon_bounds[1] = lon_id*dlon - 180.0;
    lon_bounds[0] = lon_bounds[1] - dlon;

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
    // filename = "../resources/CE2_GRAS_DEM_50m_B001_77N158W_A.tif"; 

    DEM dem(filename, nThreads); 

    World w(dem); 

    // Create the Renderer (1 thread, batch size 64)
    Renderer renderer(nThreads, 640);

    // Render the image
    std::vector<RenderedPixel> pixels = renderer.render(cam, w); 

    // Write the pixels to a PPM image file
    // makeImageLIDAR(cam, pixels); 
    makeImageDEM(cam, pixels); 
    
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

*/