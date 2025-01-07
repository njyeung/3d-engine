#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <strstream>
#include <signal.h>
using namespace std;

float Utils::dotProduct(vector3d a, vector3d b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
void Utils::crossProduct(vector3d a, vector3d b, vector3d& res) {
    res.x = (a.y * b.z - a.z*b.y);
    res.y = -(a.x*b.z - a.z*b.x);
    res.z = (a.x*b.y - a.y* b.x);
}
float Utils::magnitude(vector3d a) {
    return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
}
void Utils::normalize(vector3d a, vector3d& res) {
    float mag = magnitude(a);
    res.x = res.x/mag;
    res.y = res.y/mag;
    res.z = res.z/mag;
}
void Utils::flip(vector3d a, vector3d& res) {
    res.x = -a.x;
    res.y = -a.y;
    res.z = -a.z;
}
void Utils::loadFromObj(string filename, mesh& obj) {

    ifstream f(filename);
    if(!f.is_open()) {
        std::cout << "FILE COULD NOT BE FOUND" << std::endl;
        raise(9);
    }

    vector<vector3d> vectors;

    while(!f.eof()) {
        char line[128];
        char junk;

        f.getline(line, 128);

        strstream s;
        s << line;

        if(line[0] == 'v') {
            vector3d v;
            s >> junk >> v.x >>v.y >> v.z;
            vectors.push_back(v);
        }
        else if(line[0] == 'f') {
            int f[3];
            s >> junk >> f[0] >> f[1] >> f[2];
            vector3d v1 = vectors[f[0]-1];
            vector3d v2 = vectors[f[1]-1];
            vector3d v3 = vectors[f[2]-1];
            triangle newTri = {v1, v2, v3};
            obj.triangles.push_back(newTri);
        }
        else {
            // continue to next line
        }
    }
}
void Utils::line(vector3d a, vector3d b, vector3d& res) {
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
}

vector3d Utils::surfaceNormal(triangle a){
    vector3d normal, line1, line2;
    line(a.points[1], a.points[0], line1);
    line(a.points[2], a.points[0], line2);

    Utils::crossProduct(line1, line2, normal);
    Utils::normalize(normal, normal);
    return normal;
}
