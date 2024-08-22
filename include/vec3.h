#ifndef VEC3_H 
#define VEC3_H

#include <iostream> 

class vec3 {

    public: 

        double e[3]; 

        vec3();
        vec3(double e0, double e1, double e2);

        double x() const;
        double y() const;
        double z() const;

        vec3 operator-() const;
        double operator[](int i) const;
        double& operator[](int i);

        vec3& operator+=(const vec3& v);
        vec3& operator*=(double t);
        vec3& operator/=(double t);

        double norm() const;
        double norm2() const;

};

std::ostream& operator<<(std::ostream& out, const vec3& v);

vec3 operator+(const vec3& u, const vec3& v);
vec3 operator-(const vec3& u, const vec3& v);

vec3 operator*(const vec3& u, const vec3& v);
vec3 operator*(double t, const vec3& v);
vec3 operator*(const vec3& v, double t);

vec3 operator/(const vec3& v, double t);

vec3 unit_vector(const vec3& v);

double dot(const vec3& u, const vec3& v);
vec3 cross(const vec3& u, const vec3& v); 

// point3 is an alias for vec3 
using point3 = vec3;

#endif 