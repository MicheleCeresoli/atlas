
#include "vec2.h"
#include <cmath>

// Constructors 
vec2::vec2() : e{0, 0} {}
vec2::vec2(double e0, double e1) : e{e0, e1} {}

// Getters
double vec2::x() const { return e[0]; }
double vec2::y() const { return e[1]; }

// Operation Overloads
vec2 vec2::operator-() const { 
    return vec2(-e[0], -e[1]); 
}

vec2& vec2::operator+=(const vec2& v) {
    e[0] += v[0]; 
    e[1] += v[1]; 
    return *this; 
}

vec2& vec2::operator*=(double t) {
    e[0] *= t; 
    e[1] *= t;
    return *this; 
}

vec2& vec2::operator/=(double t) {
    return *this *= 1/t; 
}


double vec2::norm() const {
    return std::sqrt(norm2());
}

double vec2::norm2() const {
    return e[0]*e[0] + e[1]*e[1]; 
}

std::string vec2::toString() const {
    return "[" + std::to_string(e[0]) + ", " + std::to_string(e[1]) + "]";
}


// Vector Utilities 

std::ostream& operator<<(std::ostream& out, const vec2& v){
    return out << v.toString();
}

vec2 operator+(const vec2& u, double t) {
    return vec2(u[0] + t, u[1] + t);
}

vec2 operator+(const vec2& u, const vec2& v){
    return vec2(u[0] + v[0], u[1] + v[1]);
}

vec2 operator-(const vec2& u, double t) {
    return vec2(u[0] - t, u[1] - t);
}

vec2 operator-(const vec2& u, const vec2& v){
    return vec2(u[0] - v[0], u[1] - v[1]);
}

vec2 operator*(const vec2& u, const vec2& v){
    return vec2(u[0]*v[0], u[1]*v[1]);
}

vec2 operator*(double t, const vec2& v){
    return vec2(t*v[0], t*v[1]); 
}

vec2 operator*(const vec2& v, double t){
    return t*v; 
}

vec2 operator/(const vec2& v, double t){
    return (1/t)*v; 
}

vec2 unit_vector(const vec2& v){
    return v / v.norm(); 
}

double dot(const vec2& u, const vec2& v){
    return u[0]*v[0] + u[1]*v[1];
}

