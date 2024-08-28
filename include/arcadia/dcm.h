#ifndef DCM_H 
#define DCM_H

#include "vec3.h"

#include <iostream>
#include <string>

class dcm {

    public: 

        double e[9]; 

        dcm();
        dcm(double* p); 
        dcm(double e0, double e1, double e2, double e3, double e4, double e5, 
            double e6, double e7, double e8);
        
        double operator[](int i) const; 
        double& operator[](int i); 

        std::string toString() const;

        double det() const;
        double trace() const;

        dcm transpose() const; 

};


// Display Utilities 
std::ostream& operator<<(std::ostream& out, const dcm& A);

// Matrix Operations 
dcm operator*(const dcm& A, const dcm&B); 

vec3 operator*(const dcm& A, const vec3& v);

// Conversion functions
dcm angle2dcm(const std::string& ax, double x); 
dcm angle2dcm(const std::string& rot_seq, double x, double y); 
dcm angle2dcm(const std::string& rot_seq, double x, double y , double z); 


#endif 