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
vector3d Utils::vectorScale(vector3d a, float b)
{
    vector3d res;
    res.x = a.x * b;
    res.y = a.y * b;
    res.z = a.z * b;
    return res;
}
vector3d Utils::lineIntersectsPlane(vector3d plane, vector3d planeNormal, vector3d lineStart, vector3d lineEnd) {
    planeNormal = Utils::normalize(planeNormal);
    float planeDot = -Utils::dotProduct(planeNormal, plane);
    float ad = Utils::dotProduct(lineStart, planeNormal);
    float bd = Utils::dotProduct(lineEnd, planeNormal);
    float t = (-planeDot - ad) / (bd - ad);
    vector3d lineStartToEnd = Utils::vectorSubtract(lineEnd, lineStart);
    vector3d lineToIntersect = Utils::vectorScale(lineStartToEnd, t);
    return Utils::vectorAdd(lineStart, lineToIntersect);
}
int Utils::clipTriangleAgainstPlane(vector3d plane, vector3d planeNormal, triangle& input, triangle& output1, triangle& output2) {
    planeNormal = Utils::normalize(planeNormal);

    vector3d* insidePoints[3];
    int numInsidePoints = 0;
    vector3d* outsidePoints[3];
    int numOutsidePoints = 0;

    float d0 = Utils::distance(plane, planeNormal, input.points[0]);
    float d1 = Utils::distance(plane, planeNormal, input.points[1]);
    float d2 = Utils::distance(plane, planeNormal, input.points[2]);

    if(d0>=0) {
        insidePoints[numInsidePoints] = &input.points[0];
        numInsidePoints++;
    }
    else {
        outsidePoints[numOutsidePoints] = &input.points[0];
        numOutsidePoints++;
    }
    if(d1>=0) {
        insidePoints[numInsidePoints] = &input.points[1];
        numInsidePoints++;
    }
    else {
        outsidePoints[numOutsidePoints] = &input.points[1];
        numOutsidePoints++;
    }
    if(d2>=0) {
        insidePoints[numInsidePoints] = &input.points[2];
        numInsidePoints++;
    }
    else {
        outsidePoints[numOutsidePoints] = &input.points[2];
        numOutsidePoints++;
    }


    if(numInsidePoints == 0) { // All points lie outside of the plane, so clip the whole triangle
        return 0;
    }
    if(numInsidePoints == 3) { // All points lie inside the plane, so no need to clip the triangle
        output1 = input;
        return 1;
    }
    if(numInsidePoints == 1 && numOutsidePoints == 2) { // One new triangle is created
        output1.points[0] = *insidePoints[0];
        output1.points[1] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[0], *outsidePoints[0]);
        output1.points[2] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[0], *outsidePoints[1]);

        return 1;
    }
    if(numInsidePoints == 2 && numOutsidePoints == 1) { // Two new triangles are created
        output1.points[0] = *insidePoints[0];
        output1.points[1] = *insidePoints[1];
        output1.points[2] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[0], *outsidePoints[0]);

        output2.points[0] = *insidePoints[1];
        output2.points[1] = output1.points[2];
        output2.points[2] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[1], *outsidePoints[0]);

        return 2;
    }
}

float Utils::distance(vector3d plane, vector3d planeNormal, vector3d point) {
    return (planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z - Utils::dotProduct(planeNormal, plane));
}