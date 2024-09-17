#ifndef AFFINE_H
#define AFFINE_H 

#include "vec2.h"
#include <iostream> 
#include <string>

/**
 * @class Affine
 * @brief Class representing an affine transformation.
 * 
 */
class Affine {

    public: 

        /**
         * @brief Construct a new Affine object representing an identity transform.
         */
        Affine(); 

        /**
         * @brief Construct a new Affine object by specifing all the parameters.
         * 
         * @param e0 pixel size in the x-direction in map units/pixel.
         * @param e1 rotation about the x-axis. 
         * @param e2 x-coordiante of the center of the upper left pixel. 
         * @param e3 rotation about the y-axis.
         * @param e4 pixel size in the y-direction in map units/pixel.
         * @param e5 y-coordinate of the center of the upper left pixel.
         */
        Affine(double e0, double e1, double e2, double e3, double e4, double e5);

        /**
         * @brief Construct a new Affine object from a vector pointer.
         * @param p pointer to a double vector of 6 elements.
         * @warning No check is performed on the actual dimension of the array.
         */
        Affine(double* p);

        // Return x and y offsets
        double xoff() const; 
        double yoff() const; 

        /**
         * @brief Compute the determinant of the transformation.
         * 
         * @return double 
         */
        double det() const; 
        
        /**
         * @brief Return a string representing the affine transformation.
         * @return std::string 
         */
        std::string toString() const; 

        /**
         * @brief Return an identity affine transformation.
         * 
         * @return Affine 
         */
        static Affine identity(); 

        /**
         * @brief Return an affine transformation that equally scales both axes.
         * 
         * @param s scale factor.
         * @return Affine 
         */
        static Affine scale(double s);

        /**
         * @brief Return an affine transformation that scales the x and y axes.
         * 
         * @param s1 scale factor on the x-axis.
         * @param s2 scale factor on the y-axis.
         * @return Affine 
         */
        static Affine scale(double s1, double s2); 

        double operator[](int i) const; 
        double& operator[](int i); 

        Affine& operator*=(const Affine& a); 

    private: 
        double e[6];

};

std::ostream& operator<<(std::ostream& out, const Affine& a);

/**
 * @brief Compute the product between two affine transformations.
 * 
 * @param a First affine transformation. 
 * @param b Second affine transformation. 
 * @return Affine 
 */
Affine operator*(const Affine& a, const Affine &b); 

/**
 * @brief Apply the affine transformation to a 2-dimensional point.
 * 
 * @param a Affine transformation. 
 * @param v 2-dimensional vector.
 * @return vec2 
 */
vec2 operator*(const Affine& a, const vec2& v);

/**
 * @brief Compute the inverse of an affine transformation.
 * 
 * @param a Input affine transformation.
 * @return Affine 
 */
Affine inverse(const Affine& a); 

#endif 