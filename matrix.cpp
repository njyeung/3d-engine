#include "matrix.h"
#include "utils.h"

// OBJECT METHODS
Matrix::Matrix(float znear, float zfar, float fov, int screenwidth, int screenheight) {
    this->zNear = znear;
    this->zFar = zfar;
    this->fovDegrees = fov;
    this->screenWidth = screenwidth;
    this->screenHeight = screenheight;
    this->aspectRatio = (float)screenheight/(float)screenwidth;
    this->inverseFovRad = 1.0f / tanf(fovDegrees * 0.5f / 180.0f * 3.14159f);

    this->projectionMatrix.matrix[0][0] = aspectRatio * inverseFovRad;
    this->projectionMatrix.matrix[1][1] = inverseFovRad;
    this->projectionMatrix.matrix[2][2] = zFar / (zFar - zNear);
    this->projectionMatrix.matrix[3][2] = (-zNear * zFar) / (zFar - zNear);
    this->projectionMatrix.matrix[2][3] = 1;
    this->projectionMatrix.matrix[3][3] = 0;
}
mat4x4 Matrix::getProjectionMatrix() {
    return projectionMatrix;
}
void Matrix::setHeight(int screenheight)
{
    aspectRatio = (float)screenheight/(float)this->screenWidth;
}
void Matrix::setWidth(int screenwidth) {
    aspectRatio = (float)this->screenHeight/(float)screenwidth;
}


// STATIC METHODS
void Matrix::MultiplyMatrixVector(vector3d &input, vector3d &output, mat4x4 m) {
    // assume the 4th value in vector is 1
    output.x = input.x * m.matrix[0][0] + input.y * m.matrix[1][0] + input.z * m.matrix[2][0] + m.matrix[3][0] * 1;
    output.y = input.x * m.matrix[0][1] + input.y * m.matrix[1][1] + input.z * m.matrix[2][1] + m.matrix[3][1] * 1;
    output.z = input.x * m.matrix[0][2] + input.y * m.matrix[1][2] + input.z * m.matrix[2][2] + m.matrix[3][2] * 1;

    float w = input.x * m.matrix[0][3] + input.y * m.matrix[1][3] + input.z * m.matrix[2][3] + m.matrix[3][3] * 1;

    if (w != 0.0f) {
        output.x = output.x / w;
        output.y = output.y / w;
        output.z = output.z / w;
    }
}
void Matrix::MultiplyMatrixTriangle(triangle& input, triangle& output, mat4x4 m) {
    MultiplyMatrixVector(input.points[0], output.points[0], m);
    MultiplyMatrixVector(input.points[1], output.points[1], m);
    MultiplyMatrixVector(input.points[2], output.points[2], m);
}
void Matrix::TranslateVector(vector3d& input, vector3d& output, vector3d t) {
    output.x = input.x + t.x;
    output.y = input.y + t.y;
    output.z = input.z + t.z;
}
void Matrix::TranslateTriangle(triangle &input, triangle &output, vector3d t) {
    TranslateVector(input.points[0], output.points[0], t);
    TranslateVector(input.points[1], output.points[1], t);
    TranslateVector(input.points[2], output.points[2], t);
}
mat4x4 Matrix::RotationMatrixX(float fTheta)
{
    mat4x4 matRotX;
    matRotX.matrix[0][0] = 1;
    matRotX.matrix[1][1] = cosf(fTheta);
    matRotX.matrix[1][2] = sinf(fTheta);
    matRotX.matrix[2][1] = -sinf(fTheta);
    matRotX.matrix[2][2] = cosf(fTheta);
    matRotX.matrix[3][3] = 1;
    return matRotX;
}
mat4x4 Matrix::RotationMatrixZ(float fTheta) {
    mat4x4 matRotZ;
    matRotZ.matrix[0][0] = cosf(fTheta);
    matRotZ.matrix[0][1] = sinf(fTheta);
    matRotZ.matrix[1][0] = -sinf(fTheta);
    matRotZ.matrix[1][1] = cosf(fTheta);
    matRotZ.matrix[2][2] = 1;
    matRotZ.matrix[3][3] = 1;
    return matRotZ;
}