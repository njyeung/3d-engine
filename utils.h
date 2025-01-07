
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
class Utils {
public:
    static float dotProduct(vector3d a, vector3d b);
    static void crossProduct(vector3d a, vector3d b, vector3d& res);
    static float magnitude(vector3d a);
    static void normalize(vector3d a, vector3d& res);
    static void flip(vector3d a, vector3d& res);
    static void loadFromObj(string filename, mesh& obj);
    static void line(vector3d a, vector3d b, vector3d& res);
    static vector3d surfaceNormal(triangle a);
};
#endif