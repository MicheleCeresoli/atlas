#ifndef RAY_H 
#define RAY_H 

#include "vec3.h"

/** 
 * @class Ray 
 * @brief Class representing a ray object. 
 */
class Ray {
    public: 
    
        Ray(const point3& origin, const vec3& direction);

        const point3& origin() const;
        const vec3& direction() const;

        point3 at(double t) const;

        double minDistance() const;
        void getParameters(double r, double& tMin, double& tMax) const;

    private: 
        point3 p; 
        vec3 d; 

        // Useful quantities
        double pd; 
        double pd2; 
        double p2; 
};

#endif 