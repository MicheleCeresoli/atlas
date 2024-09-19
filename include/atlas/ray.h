#ifndef RAY_H 
#define RAY_H 

#include "vec3.h"

/** 
 * @class Ray 
 * @brief Class representing a ray object. 
 */
class Ray {
    public: 

        /**
         * @brief Construct a new Ray object.
         * 
         * @param origin Ray origin point.
         * @param direction Ray direction. The constructor will automatically take care of 
         * normalising the input vector.
         */
        Ray(const point3& origin, const vec3& direction);

        /**
         * @brief Return the ray origin point.
         * @return const point3& origin.
         */
        const point3& origin() const;

        /**
         * @brief Return the ray direction.
         * @return const vec3& ray direction.
         */
        const vec3& direction() const;

        /**
         * @brief Compute the position along the ray at a given distance from the origin.
         * 
         * @param t Distance from the origin, in world units, at which to evalute the ray 
         * position. If a negative value is provided, the position will be computed in a 
         * direction opposite to the one provided.
         * @return point3 3-dimensional position.
         */
        point3 at(double t) const;
        
        /**
         * @brief Return the minimum distance, in world units, from the center of the world.
         * @return double Minimum ray distance. 
         */
        double minDistance() const;

        /**
         * @brief Return the parametric values that correspond to a given distance of the 
         * ray from the world origin.
         * 
         * @param r distance from world origin.
         * @param tMin t-value at the first crossing.
         * @param tMax t-value at the second crossing. 
         */
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