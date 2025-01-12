#ifndef MATRIX_H
#define MATRIX_H
#include "utils.h"
using namespace std;

class Matrix {
private:
    float zNear;
    float zFar;
    float fovDegrees;
    float aspectRatio;
    float inverseFovRad;
    int screenWidth;
    int screenHeight;
    mat4x4 projectionMatrix;
public:
    Matrix(float znear, float zfar, float pov, int screenwidth, int screenheight);
    mat4x4 getProjectionMatrix();
    void setHeight(int screenheight);
    void setWidth(int screenWidth);
    
    /*
        Makes a translation matrix
    */
    static mat4x4 TranslationMatrix(float x, float y, float z);
    /*
        Makes a rotation X matrix
    */
    static mat4x4 RotationMatrixX(float fTheta);
    /*
        Makes a rotation Y matrix
    */
    static mat4x4 RotationMatrixY(float fTheta);
    /*
        Makes a rotation Z matrix
    */
    static mat4x4 RotationMatrixZ(float fTheta);
    /*
        Multiplies a vector by a matrix
    */
    static vector3d MultiplyVectorMatrix(vector3d input, mat4x4 m);
    /*
        Multiplies each vector of the triangle by the matrix
    */
    static triangle MultiplyTriangleMatrix(triangle input, mat4x4 m);
    /*
        Multiplies 2 matrices
    */
    static mat4x4 MultiplyMatrixMatrix(mat4x4 a, mat4x4 b);
    
};
#endif