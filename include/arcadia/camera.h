#ifndef CAMERA_H 
#define CAMERA_H 

#include "vec3.h"
#include "dcm.h"
#include "pixel.h"
#include "ray.h"

class Camera {
    
    public: 

        // Constructors
        Camera() = default;
        Camera(uint res_x, uint res_y, double fov_x, double fov_y);
        Camera(uint res, double fov);

        // Functions 

        inline const dcm& get_dcm() const {return A; }
        inline const point3& get_pos() const { return center; }

        inline void set_dcm(const dcm& orientation) { A = orientation; }
        inline void set_pos(const point3& pos) { center = pos; }

        inline uint width() const { return _width; }
        inline uint height() const { return _height; }
        inline uint nPixels() const { return _width*_height; }

        inline uint pixel_id(const uint& u, const uint& v) const { return u + _width*v; }; 
        inline uint pixel_id(const Pixel& p) const { return pixel_id(p[0], p[1]); }

        void pixel_coord(const uint& id, uint& u, uint& v) const; 

        Ray get_ray(double u, double v) const;


    private: 

        uint _width = 0;
        uint _height = 0;

        double fov_x; 
        double fov_y; 

        double dfov_x; 
        double dfov_y; 

        point3 center;  // Camera center 
        dcm A;          // Camera orientation, rotates a vector from the camera axes to the 
                        // world axes.

};

#endif 