#ifndef AFFINE_H
#define AFFINE_H 

#include "vec2.h"
#include <iostream> 

class Affine {

    public: 

        Affine(); 
        Affine(double e0, double e1, double e2, double e3, double e4, double e5);
        Affine(double* p);

        double operator[](int i) const; 
        double& operator[](int i); 

        Affine& operator*=(const Affine& a); 

        // Return x and y offsets
        double xoff() const; 
        double yoff() const; 

        // Compute the determinant of the transformation
        double det() const; 

        static Affine identity(); 
        static Affine scale(double s);
        static Affine scale(double s1, double s2); 

    private: 
        double e[6];

};

std::ostream& operator<<(std::ostream& out, const Affine& a);

Affine operator*(const Affine& a, const Affine &b); 
vec2 operator*(const Affine& a, const vec2& v);

Affine inverse(const Affine& a); 

#endif 