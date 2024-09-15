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
         * @param e0 
         * @param e1 
         * @param e2 
         * @param e3 
         * @param e4 
         * @param e5 
         */
        Affine(double e0, double e1, double e2, double e3, double e4, double e5);

        /**
         * @brief Construct a new Affine object from a vector pointer.
         * @param p 
         */
        Affine(double* p);

        // Return x and y offsets
        double xoff() const; 
        double yoff() const; 

        /**
         * @brief Compute the determinant of the transformation
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
         * @brief 
         * 
         * @return Affine 
         */
        static Affine identity(); 

        /**
         * @brief 
         * 
         * @param s 
         * @return Affine 
         */
        static Affine scale(double s);

        /**
         * @brief 
         * 
         * @param s1 
         * @param s2 
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

Affine operator*(const Affine& a, const Affine &b); 
vec2 operator*(const Affine& a, const vec2& v);

/**
 * @brief Compute the inverse of an affine transformation.
 * 
 * @param a Input affine transformation.
 * @return Affine 
 */
Affine inverse(const Affine& a); 

#endif 