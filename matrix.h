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
public:
    mat4x4 projectionMatrix;
    void setHeight(int screenheight);
    void setWidth(int screenWidth);
    Matrix(float znear, float zfar, float pov, int screenwidth, int screenheight);

    static void MultiplyMatrixVector(vector3d& input, vector3d& output, mat4x4 m);
    static void MultiplyMatrixTriangle(triangle& input, triangle& output, mat4x4 m);
    static void TranslateVector(vector3d& input, vector3d& output, vector3d t);
    static void TranslateTriangle(triangle& input, triangle& output, vector3d t);
};
#endif