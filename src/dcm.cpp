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


dcm dcm::transpose() const {
    return dcm(e[0], e[3], e[6], e[1], e[4], e[7], e[2], e[5], e[8]);
}


// Display Utilities 

std::ostream& operator<<(std::ostream& out, const dcm& A){
    return out << A[0] << ' ' << A[1] << ' ' << A[2] << '\n' 
               << A[3] << ' ' << A[4] << ' ' << A[5] << '\n' 
               << A[6] << ' ' << A[7] << ' ' << A[8] << '\n';
}


// Matrix Operations 

dcm operator*(const dcm& A, const dcm&B) {
    return dcm(
        A[0]*B[0] + A[1]*B[3] + A[2]*B[6], 
        A[0]*B[1] + A[1]*B[4] + A[2]*B[7], 
        A[0]*B[2] + A[1]*B[5] + A[2]*B[8],
        A[3]*B[0] + A[4]*B[3] + A[5]*B[6], 
        A[3]*B[1] + A[4]*B[4] + A[5]*B[7], 
        A[3]*B[2] + A[4]*B[5] + A[5]*B[8],
        A[6]*B[0] + A[7]*B[3] + A[8]*B[6], 
        A[6]*B[1] + A[7]*B[4] + A[8]*B[7], 
        A[6]*B[2] + A[7]*B[5] + A[8]*B[8]
    );
}

vec3 operator*(const dcm& A, const vec3& v){
    return vec3(
        A[0]*v[0] + A[1]*v[1] + A[2]*v[2], 
        A[3]*v[0] + A[4]*v[1] + A[5]*v[2],
        A[6]*v[0] + A[7]*v[1] + A[8]*v[2]
    );
}

// Conversion functions 

dcm angle2dcm(const std::string& ax, double x) {

    double s = sin(x), c = cos(x); 

    if (ax == "X") {
        return dcm(
            1.0, 0.0, 0.0, 
            0.0, c, s, 
            0.0, -s, c
        );
            
    } else if (ax == "Y") {
        return dcm(
            c, 0.0, -s, 
            0.0, 1.0, 0.0, 
            s, 0.0, c
        );

    } else if (ax == "Z") {
        return dcm(
             c, s, 0.0, 
            -s, c, 0.0, 
            0.0, 0.0, 1.0
        );

    } else {
        std::invalid_argument("invalid rotation axis"); 
    }

    return dcm();

}

dcm angle2dcm(const std::string& rot_seq, double x, double y) {

    double s1 = sin(x), c1 = cos(x); 
    double s2 = sin(y), c2 = cos(y); 

    if (rot_seq == "XY")
        return dcm(
             c2,  s1*s2, -c1*s2, 
            0.0,     c1,     s1, 
             s2, -s1*c2,  c1*c2
        );

    else if (rot_seq == "XZ")
        return dcm(
             c2, c1*s2, s1*s2,
            -s2, c1*c2, s1*c2, 
            0.0,   -s1,    c1
        );

    else if (rot_seq == "YX")
        return dcm(
               c1, 0.0,   -s1, 
            s1*s2,  c2, c1*s2, 
            s1*c2, -s2, c1*c2
        );

    else if (rot_seq == "YZ")
        return dcm(
             c1*c2,  s2, -s1*c2, 
            -c1*s2,  c2,  s1*s2, 
                s1, 0.0,     c1
        ); 

    else if (rot_seq == "ZX")
        return dcm(
                c1,     s1, 0.0, 
            -c2*s1,  c2*c1,  s2, 
             s2*s1, -s2*c1,  c2
        );

    else if (rot_seq == "ZY")
        return dcm(
            c2*c1, c2*s1, -s2, 
              -s1,    c1, 0.0, 
            s2*c1, s2*s1,  c2
        );

    else {
        std::invalid_argument("invalid rotation sequence");         
    }

    return dcm();

}

dcm angle2dcm(const std::string& rot_seq, double x, double y , double z) {

    double s1 = sin(x), c1 = cos(x); 
    double s2 = sin(y), c2 = cos(y); 
    double s3 = sin(z), c3 = cos(z); 

    if (rot_seq == "ZYX") {
        return dcm(
                 c2 * c1,                c2 * s1,             -s2 ,
            s3 * s2 * c1 - c3 * s1, s3 * s2 * s1 + c3 * c1, s3 * c2,
            c3 * s2 * c1 + s3 * s1, c3 * s2 * s1 - s3 * c1, c3 * c2
        );

    } else if (rot_seq == "XYX") {
        return dcm(
              c2,               s1 * s2,               -c1 * s2,
            s2 * s3, -s1 * c2 * s3 + c1 * c3, c1 * c2 * s3 + s1 * c3,
            s2 * c3, -s1 * c3 * c2 - c1 * s3, c1 * c3 * c2 - s1 * s3
        );

    } else if (rot_seq == "XYZ") {
        return dcm(
            c2 * c3,  s1 * s2 * c3 + c1 * s3, -c1 * s2 * c3 + s1 * s3,
            -c2 * s3, -s1 * s2 * s3 + c1 * c3,  c1 * s2 * s3 + s1 * c3,
                s2,             -s1 * c2,                 c1 * c2
        );

    } else if (rot_seq == "XZX") {
        return dcm(
               c2,               c1 * s2,                 s1 * s2,
            -s2 * c3,  c1 * c3 * c2 - s1 * s3,  s1 * c3 * c2 + c1 * s3,
             s2 * s3, -c1 * c2 * s3 - s1 * c3, -s1 * c2 * s3 + c1 * c3
        ); 

    } else if (rot_seq == "XZY") {
        return dcm(
            c3 * c2, c1 * c3 * s2 + s1 * s3, s1 * c3 * s2 - c1 * s3,
              -s2,             c1 * c2,                s1 * c2,
            s3 * c2, c1 * s2 * s3 - s1 * c3, s1 * s2 * s3 + c1 * c3
        );

    } else if (rot_seq == "YXY") {
        return dcm(
            -s1 * c2 * s3 + c1 * c3,  s2 * s3, -c1 * c2 * s3 - s1 * c3,
                       s1 * s2,         c2,              c1 * s2,
             s1 * c3 * c2 + c1 * s3, -s2 * c3,  c1 * c3 * c2 - s1 * s3
        ); 

    } else if (rot_seq == "YXZ") {
        return dcm(
            c1 * c3 + s2 * s1 * s3, c2 * s3, -s1 * c3 + s2 * c1 * s3,
            -c1 * s3 + s2 * s1 * c3, c2 * c3,  s1 * s3 + s2 * c1 * c3,
                  s1 * c2,             -s2,         c2 * c1
        ); 

    } else if (rot_seq == "YZX") {
        return dcm(
                       c1 * c2,         s2,              -s1 * c2,
            -c3 * c1 * s2 + s3 * s1,  c2 * c3,  c3 * s1 * s2 + s3 * c1,
             s3 * c1 * s2 + c3 * s1, -s3 * c2, -s3 * s1 * s2 + c3 * c1
        ); 

    } else if (rot_seq == "YZY") {
        return dcm(
            c1 * c3 * c2 - s1 * s3, s2 * c3, -s1 * c3 * c2 - c1 * s3,
                -c1 * s2,             c2,               s1 * s2,
            c1 * c2 * s3 + s1 * c3, s2 * s3, -s1 * c2 * s3 + c1 * c3
        ); 

    } else if (rot_seq == "ZXY") {
        return dcm( 
            c3 * c1 - s2 * s3 * s1, c3 * s1 + s2 * s3 * c1, -s3 * c2,
                -c2 * s1,                c2 * c1,              s2,
            s3 * c1 + s2 * c3 * s1, s3 * s1 - s2 * c3 * c1,  c2 * c3
        ); 

    } else if (rot_seq == "ZXZ") {
        return dcm(
            -s1 * c2 * s3 + c1 * c3, c1 * c2 * s3 + s1 * c3, s2 * s3,
            -s1 * c3 * c2 - c1 * s3, c1 * c3 * c2 - s1 * s3, s2 * c3,
                       s1 * s2,               -c1 * s2,         c2
        ); 

    } else if (rot_seq == "ZYZ") {
        return dcm(
             c1 * c3 * c2 - s1 * s3,  s1 * c3 * c2 + c1 * s3, -s2 * c3,
            -c1 * c2 * s3 - s1 * c3, -s1 * c2 * s3 + c1 * c3,  s2 * s3,
                       c1 * s2,                 s1 * s2,          c2
        );

    } else {
        std::invalid_argument("invalid rotation sequence.");
    }

    return dcm(); 
}

