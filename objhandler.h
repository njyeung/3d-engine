#ifndef OBJHANDLER_H
#define OBJHANDLER_H
#include <vector>
#include <iostream>
#include "utils.h"
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
};  
#endif
