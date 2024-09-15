// #include "atlas.h"
// #include "camera.h"
// #include "utils.h"

#include <iostream>

// #include <string>

// dcm pos2dcm(point3 pos)
// {
//     vec3 uz = -unit_vector(pos);
    
//     vec3 k(0.0, 0.0, 1.0); 
//     vec3 ux = unit_vector(cross(k, uz)); 
//     vec3 uy = unit_vector(cross(uz,ux)); 

//     return dcm(
//         ux[0], uy[0], uz[0], 
//         ux[1], uy[1], uz[1], 
//         ux[2], uy[2], uz[2]);

// }

int main(int argc, const char* argv[]) {

    // RayTracerOptions opts(6, LogLevel::DETAILED);

    // std::vector<std::string> dem_files; 
    // std::vector<std::string> dom_files; 

    // dem_files.push_back(
    //     "/home/michele/phd/snoopy/resources/dem/CE2_GRAS_DEM_50m_B001_77N158W_A.tif"
    // );
    
    // dom_files.push_back(
    //     "/home/michele/phd/snoopy/resources/dom/CE2_GRAS_DOM_50m_B001_77N158W_A.tif"
    // );

    // opts.optsWorld.demFiles = dem_files; 
    // opts.optsWorld.domFiles = dom_files; 

    // LunarRayTracer tracer(opts); 

    // double lon = deg2rad(-161); 
    // double lat = deg2rad(77); 

    // double r = 50e3 + 1737400;

    // vec3 camPos = sph2car(vec3(r, lon, lat));
    // dcm  camDCM = pos2dcm(camPos);

    // PinholeCamera cam(640, deg2rad(40));
    // cam.setPos(camPos);
    // cam.setDCM(camDCM);

    // tracer.updateCamera(&cam); 
    // tracer.run(); 

    // tracer.generateImageOptical("00000.png");
    // tracer.generateGCPs("00000.csv", 16);

    return 0;

}