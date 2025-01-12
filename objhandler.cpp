#include "objhandler.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <strstream>

void OBJHandler::appendFromObj(string filename, mesh & obj) {
    ifstream f(filename);
    if(!f.is_open()) {
        throw std::runtime_error("FILE COULD NOT BE FOUND: " + filename);
    }

    vector<vector3d> vectors;

    while(!f.eof()) {
        char rawLine[128];

        f.getline(rawLine, 128);
        strstream line;
        line << rawLine;

        std::string prefix;
        line >> prefix;

        // prefix: v, f
        // line:    x y z
        //          v1 v2 v3

        if(prefix == "v") {
            vector3d v;
            line >> v.x >> v.y >> v.z;
            vectors.push_back(v);
        }
        else if(prefix == "f") {
            ///
            vector<int> vertexIndicies;
            vector<int> vertexNormals;

            // Read each vertex definition into currVertex
            // "v1//vn1" loop 1
            // "v2//vn2" loop 2
            // ...
            std::string currVertex;
            while(line >> currVertex) {
                size_t firstSlash = currVertex.find('/');
                size_t secondSlash = currVertex.find('/', firstSlash + 1);

                if(firstSlash == std::string::npos) { // Format: v1 v2 v3
                    vertexIndicies.push_back(stoi(currVertex));
                }
                else if(secondSlash == firstSlash +1) { // Format: v1//vn1 v2//vn2 v3//vn3
                    std::string vIndex = currVertex.substr(0, firstSlash);

                    // NOT BEING USED RN
                    std::string nIndex = currVertex.substr(secondSlash + 1); 

                    vertexIndicies.push_back(stoi(vIndex));
                }
                else { // Format: v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                    std::string vIndex = currVertex.substr(0, firstSlash);

                    // NOT BEING USED RN
                    std::string normalIndex = currVertex.substr(secondSlash + 1);
                    std::string tIndex = currVertex.substr(firstSlash + 1, secondSlash - firstSlash - 1);

                    vertexIndicies.push_back(stoi(vIndex));
                }
            }

            if(vertexIndicies.size() != 3) {
                throw std::runtime_error("Faces must contain only 3 vertices");
            }

            triangle tri = {
                vectors[vertexIndicies[0]-1],
                vectors[vertexIndicies[1]-1],
                vectors[vertexIndicies[2]-1]
            };

            obj.triangles.push_back(tri);
        }
        else {
            // continue to next line
        }
    }
}

mesh OBJHandler::loadFromObj(string filename) {
    mesh toreturn;
    appendFromObj(filename, toreturn);
    return toreturn;
}