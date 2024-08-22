#ifndef DCM_H 
#define DCM_H

#include "vec3.h"
#include <iostream>

class dcm {

    public: 

        double e[9]; 

        dcm();
        dcm(double e0, double e1, double e2, double e3, double e4, double e5, 
            double e6, double e7, double e8);
        
        double operator[](int i) const; 
        double& operator[](int i); 

        double det() const;
        double trace() const;

};


// Display Utilities 
std::ostream& operator<<(std::ostream& out, const dcm& A);

// Matrix - Vector Utilities 
vec3 operator*(const dcm& A, const vec3& v);

#endif 