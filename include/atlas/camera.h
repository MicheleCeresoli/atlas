#ifndef CAMERA_H 
#define CAMERA_H 

#include "vec3.h"
#include "dcm.h"
#include "pixel.h"
#include "ray.h"


class Camera {

    public: 
        
        Camera(uint width, uint height);

        virtual ~Camera() = default;

        // Default settings
        inline uint width() const { return _width; }
        inline uint height() const { return _height; }
        inline uint nPixels() const { return _width*_height; }

        // Camera world placement 
        inline void setDCM(const dcm& orientation) { _dcm = orientation; }
        inline void setPos(const point3& pos) { _pos = pos; }

        inline const dcm& getDCM() const { return _dcm; } 
        inline const point3& getPos() const { return _pos; }

        // Pixel utilities 
        inline uint getPixelId(const uint& u, const uint& v) const { return u + _width*v; }
        inline uint getPixelId(const Pixel& p) const { return getPixelId(p[0], p[1]);}

        void getPixelCoordinates(const uint& id, uint& u, uint& v) const; 

        // Ray shooter 
        virtual Ray getRay(double u, double v, bool center = false) const = 0;

        virtual bool hasAntiAliasing() const = 0;
        virtual bool hasDefocusBlur() const = 0;


    protected: 

        point3 _pos; 
        dcm _dcm;

    private: 

        uint _width = 0; 
        uint _height = 0; 

};



class PinholeCamera : public Camera {

    public: 

        PinholeCamera(uint res, double fov); 

        Ray getRay(double u, double v, bool center = false) const override; 

        inline bool hasAntiAliasing() const override { return true; }
        inline bool hasDefocusBlur() const override { return false; }

    private: 

        double fov;
        double scale; 



};


class RealCamera : public Camera {

    public: 

        RealCamera(uint res, double focalLen, double sensorSize, double fstop);

        Ray getRay(double u, double v, bool center = false) const override; 

        inline bool hasAntiAliasing() const override { return false; }
        inline bool hasDefocusBlur() const override { return true; }

    private: 

        double focalLength;     // (mm)
        double sensorSize;      // Camera film width  (mm)

        double fov; 
        double scale;

        double pixSize;         // Physical pixel size (mm)

        double fstop;           // Defined as Aperture = focal_length / fstop
        double aperture;        

};

#endif 