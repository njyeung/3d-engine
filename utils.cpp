#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <strstream>
using namespace std;

float Utils::dotProduct(vector3d a, vector3d b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
vector3d Utils::crossProduct(vector3d a, vector3d b) {
    vector3d res;
    res.x = (a.y * b.z - a.z*b.y);
    res.y = -(a.x*b.z - a.z*b.x);
    res.z = (a.x*b.y - a.y* b.x);
    return res;
}
float Utils::magnitude(vector3d a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}
vector3d Utils::normalize(vector3d a) {
    vector3d res;
    float mag = magnitude(a);
    res.x = a.x/mag;
    res.y = a.y/mag;
    res.z = a.z/mag;
    return res;
}
vector3d Utils::vectorFlip(vector3d a) {
    vector3d res;
    res.x = -a.x;
    res.y = -a.y;
    res.z = -a.z;
    return res;
}
vector3d Utils::vectorSubtract(vector3d a, vector3d b) {
    vector3d res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}
vector3d Utils:: vectorAdd(vector3d a, vector3d b) {
    vector3d res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}
vector3d Utils::surfaceNormal(triangle a) {
    vector3d normal;
    vector3d line1 = vectorSubtract(a.points[1], a.points[0]);
    vector3d line2 = vectorSubtract(a.points[2], a.points[0]);

    normal = Utils::crossProduct(line1, line2);
    normal = Utils::normalize(normal);
    return normal;
}
vector3d Utils::vectorScale(vector3d a, float b) {
    vector3d res;
    res.x = a.x * b;
    res.y = a.y * b;
    res.z = a.z * b;
    return res;
}