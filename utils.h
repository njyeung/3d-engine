
#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <iostream>
using namespace std;

struct vector3d {
    float x;
    float y;
    float z;
};
struct triangle {
    vector3d points[3];
};
struct mesh {
    vector<triangle> triangles;
};
struct mat4x4 {
    float matrix[4][4] = {0};
};
#endif