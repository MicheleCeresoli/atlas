#ifndef LUNARRAYTRACER_H
#define LUNARRAYTRACER_H 

#include "camera.h"
#include "world.h"
#include "renderer.h"
#include "settings.h"

#include "opencv2/opencv.hpp"

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
        bool generateImageOptical(const std::string& filename, int type = CV_8UC1); 
        bool generateImageDEM(const std::string& filename, int type = CV_8UC1); 
        bool generateDepthMap(const std::string& filename, int type = CV_8UC1);

        void exportRayTracedInfo(const std::string& filename); 
        void importRayTracedInfo(const std::string& filename);
        
        // Ground Control Point Generation
        void generateGCPs(const std::string& filename, int stride); 

        // Settings Retrieval
        double getAltitude(); 

    private: 

        World world; 
        Renderer renderer; 
        Camera* cam; 
        
        LogLevel logLevel;

        void sampleAltitude();
        void checkCamPointer(); 
        void checkRenderStatus(); 

};


#endif 