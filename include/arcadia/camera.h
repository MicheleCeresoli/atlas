#ifndef CAMERA_H 
#define CAMERA_H 

#include "vec3.h"
#include "dcm.h"
#include "pixel.h"
#include "ray.h"

class Camera {
    
    public: 

        const uint width; 
        const uint height; 

        // Constructors
        
        Camera(uint res_x, uint res_y, double fov_x, double fov_y);
        Camera(uint res, double fov);

        // Functions 

        const dcm& get_dcm() const;
        const point3& get_pos() const;

        void set_dcm(const dcm& orientation);
        void set_pos(const point3& pos);

        uint nPixels() const;

        uint pixel_id(const uint& u, const uint& v) const; 
        uint pixel_id(const Pixel& p) const;

        void pixel_coord(const uint& id, uint& u, uint& v); 

        Ray get_ray(double u, double v) const;


    private: 

        double fov_x; 
        double fov_y; 

        double dfov_x; 
        double dfov_y; 

        point3 center;  // Camera center 
        dcm A;          // Camera orientation, rotates a vector from the camera axes to the 
                        // world axes.

};

#endif 