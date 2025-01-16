
#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <iostream>
using namespace std;
/*
    Additional w attribute is set to 1 by default
*/
struct vector3d {
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;
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
    static vector3d crossProduct(vector3d a, vector3d b);
    static float magnitude(vector3d a);
    static vector3d normalize(vector3d a);
    static vector3d vectorFlip(vector3d a);
    static vector3d vectorSubtract(vector3d a, vector3d);
    static vector3d vectorAdd(vector3d a, vector3d b);
    static vector3d vectorScale(vector3d a, float b);
    static vector3d surfaceNormal(triangle a);
    /*
        Returns the point where a line intersects with a plane
    */
    static vector3d lineIntersectsPlane(vector3d plane, vector3d planeNormal, vector3d lineStart, vector3d lineEnd);
    /*
        Returns the number of triangles generated from clipping a triangle against a plane, and populates the output references accordingly 
    */
    static int clipTriangleAgainstPlane(vector3d plane, vector3d planeNormal, triangle& input, triangle& output1, triangle& output2);
    /*
        Get the closest distance between a point and a plane
    */
    static float distance(vector3d plane, vector3d planeNormal, vector3d point);
};
#endif