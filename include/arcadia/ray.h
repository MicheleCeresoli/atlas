#ifndef RAY_H 
#define RAY_H 

#include "vec3.h"

class Ray {
    public: 
    
        Ray(const point3& origin, const vec3& direction);

        const point3& origin() const;
        const vec3& direction() const;

        point3 at(double t) const;

        double min_distance() const;
        void get_parameter(double *t, double r) const;

    private: 
        point3 p; 
        vec3 d; 

        // Useful quantities
        double pd; 
        double pd2; 
        double p2; 
};

#endif 