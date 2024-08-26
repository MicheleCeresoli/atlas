#ifndef CAMERA_H 
#define CAMERA_H 

#include "vec3.h"
#include "dcm.h"
#include "pixel.h"
#include "ray.h"

class Camera {
    
    public: 

        const int width; 
        const int height; 

        // Constructors
        
        Camera(int res_x, int res_y, double fov_x, double fov_y);
        Camera(int res, double fov);

        // Functions 

        const dcm& get_dcm() const;
        const point3& get_pos() const;

        void set_dcm(const dcm& orientation);
        void set_pos(const point3& pos);

        int pixel_id(const Pixel& p);

        Ray get_ray(int u, int v) const;


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