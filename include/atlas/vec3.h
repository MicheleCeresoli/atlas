#ifndef VEC3_H 
#define VEC3_H

#include <cmath>
#include <iostream> 
#include <string>

/**
 * @class vec3
 * @brief Class representing a 3-dimensional vector.
 */
class vec3 {

    public: 

        /**
         * @brief Array storing the vector components.
         */
        double e[3]; 

        /**
         * @brief Construct a new vec3 object with null components.
         */
        vec3();

        /**
         * @brief Construct a new vec3 object.
         * @param e0 Value of the x-component.
         * @param e1 Value of the y-component.
         * @param e2 Value of the z-component.
         */
        vec3(double e0, double e1, double e2);

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
         * @brief Return the z component of the vector.
         * @return double 
         */
        double z() const;

        /**
         * @brief Compute the Euclidean norm of the vector.
         * @return double 
         */
        inline double norm() const { return std::sqrt(norm2()); };

        /**
         * @brief Compute the squared Euclidean norm of the vector.
         * @return double 
         */
        inline double norm2() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; };

        /**
         * @brief Return a string representing the vector content.
         * @return std::string 
         */
        std::string toString() const; 

        vec3 operator-() const;
        
        inline double operator[](int i) const { return e[i]; };
        inline double& operator[](int i) { return e[i]; };

        vec3& operator+=(const vec3& v);
        vec3& operator*=(double t);
        vec3& operator/=(double t);

};

std::ostream& operator<<(std::ostream& out, const vec3& v);

vec3 operator+(const vec3& u, const vec3& v);
vec3 operator-(const vec3& u, const vec3& v);

vec3 operator*(const vec3& u, const vec3& v);
vec3 operator*(double t, const vec3& v);
vec3 operator*(const vec3& v, double t);

vec3 operator/(const vec3& v, double t);

/**
 * @brief Normalise a vector.
 * 
 * @param v Input vector to be normalised.
 * @return vec2 
 */
vec3 unit_vector(const vec3& v);

/**
 * @brief Compute the dot product between two vectors.
 * 
 * @param u First vector.
 * @param v Second vector.
 * @return double 
 */
double dot(const vec3& u, const vec3& v);

/**
 * @brief Compute the cross product between two vectors.
 * 
 * @param u First vector.
 * @param v Second vector.
 * @return vec3 
 */
vec3 cross(const vec3& u, const vec3& v); 

// point3 is an alias for vec3 
using point3 = vec3;

#endif 