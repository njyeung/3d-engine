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
    

    static mat4x4 RotationMatrixX(float fTheta);
    static mat4x4 RotationMatrixZ(float fTheta);
    static void MultiplyMatrixVector(vector3d& input, vector3d& output, mat4x4 m);
    static void MultiplyMatrixTriangle(triangle& input, triangle& output, mat4x4 m);
    static void TranslateVector(vector3d& input, vector3d& output, vector3d t);
    static void TranslateTriangle(triangle& input, triangle& output, vector3d t);
};
#endif