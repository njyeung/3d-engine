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
vector3d Matrix::MultiplyVectorMatrix(vector3d input, mat4x4 m) {
    vector3d res;

    res.x = input.x * m.matrix[0][0] + input.y * m.matrix[1][0] + input.z * m.matrix[2][0] + m.matrix[3][0] * 1;
    res.y = input.x * m.matrix[0][1] + input.y * m.matrix[1][1] + input.z * m.matrix[2][1] + m.matrix[3][1] * 1;
    res.z = input.x * m.matrix[0][2] + input.y * m.matrix[1][2] + input.z * m.matrix[2][2] + m.matrix[3][2] * 1;
    res.w = input.x * m.matrix[0][3] + input.y * m.matrix[1][3] + input.z * m.matrix[2][3] + m.matrix[3][3] * 1;

    return res;
}
triangle Matrix::MultiplyTriangleMatrix(triangle input, mat4x4 m) {
    triangle res;
    
    res.points[0] = MultiplyVectorMatrix(input.points[0], m);
    res.points[1] = MultiplyVectorMatrix(input.points[1], m);
    res.points[2] = MultiplyVectorMatrix(input.points[2], m);
    
    return res;
}
mat4x4 Matrix::MultiplyMatrixMatrix(mat4x4 a, mat4x4 b) {
    mat4x4 res;
    for(int col = 0; col<4; col++) {
        for(int row = 0; row<4; row++) {
            res.matrix[row][col] = a.matrix[row][0] * b.matrix[0][col] 
            + a.matrix[row][1] * b.matrix[1][col] 
            + a.matrix[row][2] * b.matrix[2][col] 
            + a.matrix[row][3] * b.matrix[3][col];
        }
    }
    return res;
}
mat4x4 Matrix::TranslationMatrix(float x, float y, float z) {
    mat4x4 res;
    res.matrix[0][0] = 1.0f;
    res.matrix[1][1] = 1.0f;
    res.matrix[2][2] = 1.0f;
    res.matrix[3][3] = 1.0f;
    res.matrix[3][0] = x;
    res.matrix[3][1] = y;
    res.matrix[3][2] = z;
    return res;
}
mat4x4 Matrix::RotationMatrixX(float fTheta) {
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