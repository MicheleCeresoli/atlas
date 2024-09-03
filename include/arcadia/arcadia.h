#ifndef LUNARRAYTRACER_H
#define LUNARRAYTRACER_H 

#include "camera.h"
#include "world.h"
#include "renderer.h"
#include "settings.h"

#include <vector>
#include <string> 

class LunarRayTracer {

    public: 

        LunarRayTracer(RayTracerOptions opts);

        void run(); 

        // Renderer Update Routines 
        inline void updateRenderingOptions(const RenderingOptions& opts) {
            renderer.updateRenderingOptions(opts);
        }

        // Camera Update Routines
        inline void updateCamera(Camera* camera) { cam = camera; }
        inline void updateCameraPosition(const point3& pos) { cam->setPos(pos); }
        inline void updateCameraOrientation(const dcm& dcm) { cam->setDCM(dcm); } 

        // Image Generation Routines
        bool generateImageOptical(const std::string& filename); 
        bool generateImageDEM(const std::string& filename); 
        
        void generateImageLIDAR(const std::string& filename); 
        void generateImageGCPs(const std::string& filename); 

        // Settings Retrieval
        inline double getAltitude() { return altitude; }; 
        
        void getImageGCPs();


    private: 

        World world; 
        Renderer renderer;
        Camera* cam; 

        double altitude;

        void sampleAltitude();

};


#endif 