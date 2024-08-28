
#include "vec3.h"
#include <cmath>

// Constructors 
vec3::vec3() : e{0, 0, 0} {}
vec3::vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

// Getters
double vec3::x() const { return e[0]; }
double vec3::y() const { return e[1]; }
double vec3::z() const { return e[2]; }

// Operation Overloads
vec3 vec3::operator-() const { 
    return vec3(-e[0], -e[1], -e[2]); 
}

vec3& vec3::operator+=(const vec3& v) {
    e[0] += v[0]; 
    e[1] += v[1]; 
    e[2] += v[2]; 
    return *this; 
}

vec3& vec3::operator*=(double t) {
    e[0] *= t; 
    e[1] *= t; 
    e[2] *= t; 
    return *this; 
}

vec3& vec3::operator/=(double t) {
    return *this *= 1/t; 
}


double vec3::norm() const {
    return std::sqrt(norm2());
}

double vec3::norm2() const {
    return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; 
}

std::string vec3::toString() const {
    return "[" + std::to_string(e[0]) + ", " + std::to_string(e[1]) 
               + ", " + std::to_string(e[2]) + "]";
}

// Vector Utilities 

std::ostream& operator<<(std::ostream& out, const vec3& v){
    return out << v.toString(); 
}

vec3 operator+(const vec3& u, const vec3& v){
    return vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

vec3 operator-(const vec3& u, const vec3& v){
    return vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

vec3 operator*(const vec3& u, const vec3& v){
    return vec3(u[0]*v[0], u[1]*v[1], u[2]*v[2]);
}

vec3 operator*(double t, const vec3& v){
    return vec3(t*v[0], t*v[1], t*v[2]); 
}

vec3 operator*(const vec3& v, double t){
    return t*v; 
}

vec3 operator/(const vec3& v, double t){
    return (1/t)*v; 
}

vec3 unit_vector(const vec3& v){
    return v / v.norm(); 
}

double dot(const vec3& u, const vec3& v){
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

vec3 cross(const vec3& u, const vec3& v){
    return vec3(
        u[1]*v[2] - u[2]*v[1], 
        u[2]*v[0] - u[0]*v[2], 
        u[0]*v[1] - u[1]*v[0]
    );
}