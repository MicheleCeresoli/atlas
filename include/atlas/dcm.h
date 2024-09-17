#ifndef DCM_H 
#define DCM_H

#include "vec3.h"

#include <iostream>
#include <string>

/**
 * @class dcm 
 * @brief Class representing a Direction Cosine Matrix (DCM).
 */
class dcm {

    public: 

        double e[9]; 

        /**
         * @brief Construct a new identity dcm. 
         */
        dcm();

        /**
         * @brief Construct a new dcm object from a vector pointer.
         * 
         * @param p pointer to a double vector of 9 elements.
         * @warning No check is performed on the actual dimension of the array.
         */
        dcm(double* p); 

        /**
         * @brief Construct a new dcm object by specifing all the parameters.
         * 
         * @param e0 A(1,1) element.
         * @param e1 A(1,2) element.
         * @param e2 A(1,3) element.
         * @param e3 A(2,1) element.
         * @param e4 A(2,2) element.
         * @param e5 A(2,3) element.
         * @param e6 A(3,1) element.
         * @param e7 A(3,2) element.
         * @param e8 A(3,3) element.
         */
        dcm(double e0, double e1, double e2, double e3, double e4, double e5, 
            double e6, double e7, double e8);
        
        double operator[](int i) const; 
        double& operator[](int i); 

        /**
         * @brief Compute the determinant of the matrix.
         * @return double 
         * @note This class does not guarantee the actual data represents a DCM, thus the 
         * determinant could theoretically be different from unity.
         */
        double det() const;

        /**
         * @brief Compute the trace of the matrix.
         * @return double 
         */
        double trace() const;

        /**
         * @brief Return the matrix transpose.
         * 
         * @return dcm 
         */
        dcm transpose() const; 

        /**
         * @brief Return a string representing the dcm matrix.
         * @return std::string 
         */
        std::string toString() const;

};


// Display Utilities 
std::ostream& operator<<(std::ostream& out, const dcm& A);

// Matrix Operations 

/**
 * @brief Compute the product between two dcm.
 * 
 * @param A First dcm.
 * @param B Second dcm.
 * @return dcm 
 */
dcm operator*(const dcm& A, const dcm&B); 

/**
 * @brief Compute the product between a dcm and a 3-dimensional vector.
 * 
 * @param A DCM rotation matrix.
 * @param v vector to be rotated.
 * @return vec3 
 */
vec3 operator*(const dcm& A, const vec3& v);

// Conversion functions

/**
 * @brief Compute a dcm that performs a rotation around the specified axis.
 * 
 * @param ax rotation axis. Supported values are "X", "Y" or "Z".
 * @param x rotation angle, in radians. 
 * @return dcm 
 */
dcm angle2dcm(const std::string& ax, double x); 

/**
 * @brief Compute a dcm that performs a set of two rotations around the specified axes.
 * 
 * @param rot_seq rotation sequence. Supported values include "XY", "XZ", "YX", "YZ", "ZX" 
 * and "ZY".
 * @param x First rotation angle, in radians.
 * @param y Second rotation angle, in radians.
 * 
 * @note This function assigns dcm = A2 * A1, in which Ai is the DCM relatetd to the i-th 
 * rotation.
 * 
 * @return dcm 
 */
dcm angle2dcm(const std::string& rot_seq, double x, double y); 

/**
 * @brief Compute a dcm that performs a set of three rotations around the specified axes.
 * 
 * @param rot_seq rotation sequence. Supported values are "XYX", "XYZ", "XZX", "ZXY", "YXY", 
 * "YXZ", "YZX", "YZY", "ZXY", "ZXZ", "ZYX" and "ZYZ".
 * @param x First rotation angle, in radians.
 * @param y Second rotation angle, in radians.
 * @param z Third rotation angle, in radians.
 * 
 * @note This function assigns dcm = A3 * A2 * A1, in which Ai is the DCM relatetd to the i-th 
 * rotation.
 * 
 * @return dcm 
 * 
 */
dcm angle2dcm(const std::string& rot_seq, double x, double y , double z); 


#endif 