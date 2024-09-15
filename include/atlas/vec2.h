#ifndef VEC2_H 
#define VEC2_H

#include <cmath>
#include <iostream> 
#include <string>

/**
 * @class vec2 
 * @brief Class representing a 2-dimensional vector.
 */
class vec2 {

    public: 

        /**
         * @brief Array storing the vector components.
         */
        double e[2]; 

        /**
         * @brief Construct a new vec2 object with null components.
         */
        vec2();

        /**
         * @brief Construct a new vec2 object.
         * @param e0 Value of the x-component.
         * @param e1 Value of the y-component.
         */
        vec2(double e0, double e1);

        /**
         * @brief Return the x component of the vector.
         * @return double 
         */
        double x() const;
       
        /**
         * @brief Return the y component of the vector.
         * @return double 
         */
        double y() const;

        /**
         * @brief Compute the Euclidean norm of the vector.
         * @return double 
         */
        inline double norm() const { return std::sqrt(norm2()); };

        /**
         * @brief Compute the squared Euclidean norm of the vector.
         * @return double 
         */
        inline double norm2() const { return e[0]*e[0] + e[1]*e[1]; };

        /**
         * @brief Return a string representing the vector content.
         * @return std::string 
         */
        std::string toString() const; 

        vec2 operator-() const;
        
        inline double operator[](int i) const { return e[i]; };
        inline double& operator[](int i) { return e[i]; };

        vec2& operator+=(const vec2& v);
        vec2& operator*=(double t);
        vec2& operator/=(double t);

};

std::ostream& operator<<(std::ostream& out, const vec2& v);

vec2 operator+(const vec2& u, double t); 
vec2 operator+(const vec2& u, const vec2& v);

vec2 operator-(const vec2& u, double t); 
vec2 operator-(const vec2& u, const vec2& v);

vec2 operator*(const vec2& u, const vec2& v);
vec2 operator*(double t, const vec2& v);
vec2 operator*(const vec2& v, double t);

vec2 operator/(const vec2& v, double t);

/**
 * @brief Normalise a vector.
 * 
 * @param v Input vector to be normalised.
 * @return vec2 
 */
vec2 unit_vector(const vec2& v);

/**
 * @brief Compute the dot product between two vectors.
 * 
 * @param u First vector.
 * @param v Second vector.
 * @return double 
 */
double dot(const vec2& u, const vec2& v);

// point2 is an alias for vec2 
using point2 = vec2;

#endif 