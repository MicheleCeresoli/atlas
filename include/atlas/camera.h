#ifndef CAMERA_H 
#define CAMERA_H 

#include "vec3.h"
#include "dcm.h"
#include "pixel.h"
#include "ray.h"
#include "types.h"


/**
 * @class Camera
 * @brief Base class for different camera models.
 * 
 */
class Camera {

    public: 
        
        /**
         * @brief Construct a new Camera object with given width and height.
         * 
         * @param width number of pixels in the horizontal axis.
         * @param height number of pixels in the vertical axis.
         */
        Camera(ui32_t width, ui32_t height);

        virtual ~Camera() = default;

        /**
         * @brief Return the horizontal resolution, in pixels.
         * @return ui32_t 
         */
        inline ui32_t width() const { return _width; }

        /**
         * @brief Return the vertical resolution, in pixels.
         * @return ui32_t 
         */
        inline ui32_t height() const { return _height; }

        /**
         * @brief Return the total number of pixels.
         * @return ui32_t 
         */
        inline ui32_t nPixels() const { return _width*_height; }


        /**
         * @brief Update the camera orientation.
         * @param orientation New camera orientation.
         */
        inline void setDCM(const dcm& orientation) { _dcm = orientation; }

        /**
         * @brief Update the camera position in the world.
         * @param pos New camera position, in meters.
         */
        inline void setPos(const point3& pos) { _pos = pos; }

        /**
         * @brief Return a constant reference to the current camera dcm matrix.
         * @return const dcm& 
         */
        inline const dcm& getDCM() const { return _dcm; } 

        /**
         * @brief Return a constant reference to the current camera world position.
         * @return const point3& 
         */
        inline const point3& getPos() const { return _pos; }

        /**
         * @brief Compute the ID of a pixel at given coordinates in the image plane.
         * 
         * @param u Pixel horizontal coordinate.
         * @param v Pixel vertical coordinate.
         * @return ui32_t Desired pixel ID.
         */
        inline ui32_t getPixelId(const ui32_t& u, const ui32_t& v) const { return u + _width*v; }

        /**
         * @brief Compute the ID of a pixel at a given point.
         * 
         * @param p Pixel coordinates.
         * @return ui32_t Desired pixel ID.
         */
        inline ui32_t getPixelId(const Pixel& p) const { return getPixelId(p[0], p[1]);}

        /**
         * @brief Convert a pixel ID into horizontal and vertical coordinates.
         * 
         * @param id Pixel ID.
         * @param u Horizontal coordiante.
         * @param v Vertical coordinate.
         */
        void getPixelCoordinates(const ui32_t& id, ui32_t& u, ui32_t& v) const; 

        /**
         * @brief Return the ray associated to a given pixel in the image plane.
         * 
         * @param u Pixel horizontal coordinate.
         * @param v Pixel vertical coordinate.
         * @param center True if the ray should pass through the pixel center.
         * 
         * @note The `center` parameter is used to handle cases where one wants to 
         * randomise the ray target point on the image plane around the given pixel center.
         * 
         * @return Ray Desired ray object.
         */
        virtual Ray getRay(double u, double v, bool center = false) const = 0;

        /**
         * @brief Return true if the camera requires anti-aliasing.
         */
        virtual bool hasAntiAliasing() const = 0;

        /**
         * @brief Return true if the camera is subject to defocus blur effects. 
         */
        virtual bool hasDefocusBlur() const = 0;


    protected: 

        point3 _pos; 
        dcm _dcm;

    private: 
         
        ui32_t _width = 0; 
        ui32_t _height = 0; 

};


/**
 * @class PinholeCamera
 * @brief Class representing a Pinhole camera model.
 * 
 */
class PinholeCamera : public Camera {

    public: 

        /**
         * @brief Construct a new Pinhole Camera object.
         * 
         * @param res camera resolution, in pixels.
         * @param fov camera field of view, in radians.
         * 
         * @note This model currently supports only squared sensors with an equal number 
         * of pixels in the horizontal and vertical axes.
         */
        PinholeCamera(ui32_t res, double fov); 
        PinholeCamera(ui32_t width, ui32_t height, double fov_x, double fov_y); 

        Ray getRay(double u, double v, bool center = false) const override; 

        inline bool hasAntiAliasing() const override { return true; }
        inline bool hasDefocusBlur() const override { return false; }

    private: 

        double fov[2];
        double scale[2]; 



};

/**
 * @class RealCamera
 * @brief Class representing a Real camera model.
 * 
 */
class RealCamera : public Camera {

    public: 

        /**
         * @brief Construct a new Real Camera object.
         * 
         * @param res camera resolution, in pixels.
         * @param focalLen Focal length, in mm.
         * @param sensorSize Sensor size, in mm.
         * @param fstop f-stop number, used to determine the camera aperture size.
         */
        RealCamera(ui32_t res, double focalLen, double sensorSize, double fstop);
        
        RealCamera(
            ui32_t width, ui32_t height, double focalLen, double sensor_width, 
            double sensor_height, double fstop
        ); 

        /**
         * @brief Return the ray associated to a given pixel in the image plane.
         * 
         * @param u Pixel horizontal coordinate.
         * @param v Pixel vertical coordinate.
         * @param center True if the ray should pass through the pixel center.
         * 
         * @note Whenever `center` is set to `false`, the ray is generated by randomly 
         * sampling an origin point in the aperture disk and a target point around the 
         * pixel center coordinates. 
         * 
         * @return Ray Desired ray object.
         */
        Ray getRay(double u, double v, bool center = false) const override; 

        inline bool hasAntiAliasing() const override { return false; }
        inline bool hasDefocusBlur() const override { return true; }

    private: 

        double focalLength;        // (mm)
        double sensorSize[2];      // Camera film width and height (mm)

        double fov[2]; 
        double scale[2];

        double pixSize[2];         // Physical pixel width and height (mm)

        double fstop;              // Defined as Aperture = focal_length / fstop
        double aperture;        

};

#endif 