#ifndef OBJHANDLER_H
#define OBJHANDLER_H
#include <vector>
#include <iostream>
#include "utils.h"
#include "matrix.h"
using namespace std;

class OBJHandler {
public:
    /*
        returns a mesh of the obj
    */
    static mesh loadFromObj(string filename);
    /*
        appends the obj file to the mesh reference passed in.
    */
    static void appendFromObj(string filename, mesh& obj);
    /*
        returns a cube
    */
    static mesh loadCube();
};  
#endif
