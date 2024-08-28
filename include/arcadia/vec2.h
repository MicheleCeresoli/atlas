#ifndef VEC2_H 
#define VEC2_H

#include <iostream> 
#include <string>

class vec2 {

    public: 

        double e[2]; 

        vec2();
        vec2(double e0, double e1);

        double x() const;
        double y() const;

        vec2 operator-() const;
        
        inline double operator[](int i) const { return e[i]; };
        inline double& operator[](int i) { return e[i]; };

        vec2& operator+=(const vec2& v);
        vec2& operator*=(double t);
        vec2& operator/=(double t);

        double norm() const;
        double norm2() const;

        std::string toString() const; 

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

vec2 unit_vector(const vec2& v);

double dot(const vec2& u, const vec2& v);

// point2 is an alias for vec2 
using point2 = vec2;

#endif 