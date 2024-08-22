#include "dcm.h"
#include <cmath>

// Constructors
dcm::dcm() : e{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0} {}
dcm::dcm(double e0, double e1, double e2, double e3, double e4, double e5, 
         double e6, double e7, double e8) : e{e0, e1, e2, e3, e4, e5, e6, e7, e8} {}

double dcm::operator[](int i) const { return e[i]; }
double& dcm::operator[](int i ) { return e[i]; }

double dcm::det() const {
    return e[0]*(e[4]*e[8] - e[5]*e[7]) - 
           e[1]*(e[3]*e[8] - e[5]*e[6]) +
           e[2]*(e[3]*e[7] - e[4]*e[6]); 
}

double dcm::trace() const {
    return e[0] + e[4] + e[8];
}



// Display Utilities 

std::ostream& operator<<(std::ostream& out, const dcm& A){
    return out << A[0] << ' ' << A[1] << ' ' << A[2] << '\n' 
               << A[3] << ' ' << A[4] << ' ' << A[5] << '\n' 
               << A[6] << ' ' << A[7] << ' ' << A[8] << '\n';
}

// Matrix - Vector Utilities 

vec3 operator*(const dcm& A, const vec3& v){
    return vec3(
        A[0]*v[0] + A[1]*v[1] + A[2]*v[2], 
        A[3]*v[0] + A[4]*v[1] + A[5]*v[2],
        A[6]*v[0] + A[7]*v[1] + A[8]*v[2]
    );
}
