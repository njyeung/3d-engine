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
vector2d Utils::vectorScale(vector2d a, float b) {
    vector2d res;
    res.u = a.u * b;
    res.v = a.v * b;

    return res;
}
vector3d Utils::lineIntersectsPlane(vector3d plane, vector3d planeNormal, vector3d lineStart, vector3d lineEnd, float &t) {
    planeNormal = Utils::normalize(planeNormal);
    float planeDot = -Utils::dotProduct(planeNormal, plane);
    float ad = Utils::dotProduct(lineStart, planeNormal);
    float bd = Utils::dotProduct(lineEnd, planeNormal);
    t = (-planeDot - ad) / (bd - ad);
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

    vector2d* insideTextures[3];
    int numInsideTextures = 0;
    vector2d* outsideTextures[3];
    int numOutsideTextures = 0;

    // Classify whether point is beyond or within the clipping plane
    float d0 = Utils::distance(plane, planeNormal, input.points[0]);
    float d1 = Utils::distance(plane, planeNormal, input.points[1]);
    float d2 = Utils::distance(plane, planeNormal, input.points[2]);

    // Separate points of the triangle into the arrays
    if(d0>=0) {
        insidePoints[numInsidePoints] = &input.points[0];
        numInsidePoints++;
        insideTextures[numInsideTextures] = &input.texture[0];
        numInsideTextures++;
    }
    else {
        outsidePoints[numOutsidePoints] = &input.points[0];
        numOutsidePoints++;
        insideTextures[numOutsideTextures] = &input.texture[0];
        numOutsideTextures++;
    }
    if(d1>=0) {
        insidePoints[numInsidePoints] = &input.points[1];
        numInsidePoints++;
        insideTextures[numInsideTextures] = &input.texture[1];
        numInsideTextures++;
    }
    else {
        outsidePoints[numOutsidePoints] = &input.points[1];
        numOutsidePoints++;
        outsideTextures[numOutsideTextures] = &input.texture[1];
        numOutsideTextures++;
    }
    if(d2>=0) {
        insidePoints[numInsidePoints] = &input.points[2];
        numInsidePoints++;
        insideTextures[numInsideTextures] = &input.texture[2];
        numInsideTextures++;
    }
    else {
        outsidePoints[numOutsidePoints] = &input.points[2];
        numOutsidePoints++;
        outsideTextures[numOutsideTextures] = &input.texture[2];
        numOutsideTextures++;
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
        output1.texture[0] = *insideTextures[0];

        float t;
        output1.points[1] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[0], *outsidePoints[0], t);
        output1.texture[1].u = t * (outsideTextures[0]->u - insideTextures[0]->u) + insideTextures[0]->u;
		output1.texture[1].v = t * (outsideTextures[0]->v - insideTextures[0]->v) + insideTextures[0]->v;
        output1.texture[1].w = t * (outsideTextures[0]->w - insideTextures[0]->w) + insideTextures[0]->w;

        output1.points[2] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[0], *outsidePoints[1], t);
        output1.texture[2].u = t * (outsideTextures[1]->u - insideTextures[0]->u) + insideTextures[0]->u;
        output1.texture[2].v = t * (outsideTextures[1]->v - insideTextures[0]->v) + insideTextures[0]->v;
        output1.texture[2].w = t * (outsideTextures[1]->w - insideTextures[0]->w) + insideTextures[0]->w;
        return 1;
    }
    if(numInsidePoints == 2 && numOutsidePoints == 1) { // Two new triangles are created
        output1.points[0] = *insidePoints[0];
        output1.points[1] = *insidePoints[1];

        output1.texture[0] = *insideTextures[0];
        output1.texture[1] = *insideTextures[1];

        float t;
        output1.points[2] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[0], *outsidePoints[0], t);
        output1.texture[2].u = t * (outsideTextures[0]->u - insideTextures[0]->u) + insideTextures[0]->u;
        output1.texture[2].v = t * (outsideTextures[0]->v - insideTextures[0]->v) + insideTextures[0]->v;
        output1.texture[2].w = t * (outsideTextures[0]->w - insideTextures[0]->w) + insideTextures[0]->w;

        output2.points[0] = *insidePoints[1];
        output2.points[1] = output1.points[2];

        output2.texture[0] = *insideTextures[1];
        output2.texture[1] = output1.texture[2];

        output2.points[2] = Utils::lineIntersectsPlane(plane, planeNormal, *insidePoints[1], *outsidePoints[0], t);
        output2.texture[2].u = t * (outsideTextures[0]->u - insideTextures[1]->u) + insideTextures[1]->u;
        output2.texture[2].v = t * (outsideTextures[0]->v - insideTextures[1]->v) + insideTextures[1]->v;
        output2.texture[2].w = t * (outsideTextures[0]->w - insideTextures[1]->w) + insideTextures[1]->w;
        return 2;
    }
}

float Utils::distance(vector3d plane, vector3d planeNormal, vector3d point) {
    return (planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z - Utils::dotProduct(planeNormal, plane));
}