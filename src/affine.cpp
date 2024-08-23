
#include "affine.h"

// Constructors 

// The default constructor returns an identity transform
Affine::Affine() : e{1.0, 0.0, 0.0, 0.0, 1.0, 0.0} {}
Affine::Affine(double e0, double e1, double e2, double e3, double e4, 
    double e5) : e{e0, e1, e2, e3, e4, e5} {}

// Be carefull, this constructor does not check whether enough memory was allocated in p! 
Affine::Affine(double *p) : Affine(p[0], p[1], p[2], p[3], p[4], p[5]) {}

// Operation Overloads 

double Affine::operator[](int i) const { return e[i]; }
double& Affine::operator[](int i) { return e[i]; }

Affine& Affine::operator*=(const Affine& a) {

    double sa = e[0], sb = e[1], sd = e[3], se = e[4]; 

    e[0]  = sa*a[0] + sb*a[3]; 
    e[1]  = sa*a[1] + sb*a[4];
    e[2] += sa*a[2] + sb*a[5];

    e[3]  = sd*a[0] + se*a[3];
    e[4]  = sd*a[1] + se*a[4];
    e[5] += sd*a[2] + se*a[5];

    return *this; 
}

double Affine::xoff() const {
    return e[2]; 
}

double Affine::yoff() const {
    return e[5];
}

double Affine::det() const {
    return e[0]*e[4] - e[1]*e[3];
}

// Return an identity transformation
Affine Affine::identity() { return Affine(); }

// Return an affine transformation for scaling
Affine Affine::scale(double s) {
    return Affine::scale(s, s); 
}

Affine Affine::scale(double s1, double s2) {
    return Affine(s1, 0.0, 0.0, 0.0, s2, 0.0);
}

// Stream 
std::ostream& operator<<(std::ostream& out, const Affine& a) {
    return out << a[0] << ' ' << a[1] << ' ' << a[2] << std::endl 
               << a[3] << ' ' << a[4] << ' ' << a[5];
}

// Matrix / Vector utilities

Affine operator*(const Affine& a, const Affine& b) {

    return Affine(
        a[0]*b[0] + a[1]*b[3], 
        a[0]*b[1] + a[1]*b[4],
        a[0]*b[2] + a[1]*b[5] + a[2],
        a[3]*b[0] + a[4]*b[3],
        a[3]*b[1] + a[4]*b[4],
        a[3]*b[2] + a[4]*b[5] + a[5]
    );
}

vec2 operator*(const Affine& a, const vec2& v) {

    return vec2(
        a[0]*v[0] + a[1]*v[1] + a[2], 
        a[3]*v[0] + a[4]*v[1] + a[5]
    );

}

Affine inverse(const Affine& a) {

    double d = 1.0/a.det(); 

    double sa =  a[4]*d, sb = -a[1]*d; 
    double sd = -a[3]*d, se =  a[0]*d;

    return Affine(
        sa, sb, -sb*a[5] - sa*a[2], 
        sd, se, -sd*a[2] - se*a[5]
    );

}