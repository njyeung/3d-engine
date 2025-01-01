#include "olcConsoleGameEngine.h"
using namespace std;

struct vec3D {
    float x;
    float y;
    float z;
};

struct triangle {
    vec3D points[3];
};

struct mesh {
    vector<triangle> triangles;
};

struct m4x4 {
    float matrix[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine{
private:
    mesh cube;
    m4x4 projMatrix;
    float fTheta;

    void MultiplyMatrixVector(vec3D &input, vec3D &output, m4x4& m) {
        // assume the 4th value in vector is 1
        output.x = input.x * m.matrix[0][0] + input.y * m.matrix[1][0] + input.z * m.matrix[2][0] + m.matrix[3][0] * 1;
        output.y = input.x * m.matrix[0][1] + input.y * m.matrix[1][1] + input.z * m.matrix[2][1] + m.matrix[3][1] * 1;
        output.z = input.x * m.matrix[0][2] + input.y * m.matrix[1][2] + input.z * m.matrix[2][2] + m.matrix[3][2] * 1;

        float w = input.x * m.matrix[0][3] + input.y * m.matrix[1][3] + input.z * m.matrix[2][3] + m.matrix[3][3] * 1;

        if (w != 0.0f) {
            output.x = output.x / w;
            output.y = output.y / w;
            output.z = output.z / w;
        }
    }

public:
    olcEngine3D() {
        m_sAppName = L"3D Demo";
    }

public:
    bool OnUserCreate() override {

        triangle south1 = { 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,1.0f,0.0f };
        triangle south2 = { 0.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,0.0f,0.0f };

        triangle east1 = { 1.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f,1.0f };
        triangle east2 = { 1.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 1.0f,0.0f,1.0f };
        
        triangle west1 = { 0.0f,0.0f,1.0f,  0.0f,1.0f,1.0f, 0.0f,1.0f,0.0f };
        triangle west2 = { 0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f };

        triangle top1 = { 0.0f,1.0f,0.0f, 0.0f,1.0f,1.0f, 1.0f,1.0f,1.0f };
        triangle top2 = { 0.0f,1.0f,0.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,0.0f };
        
        triangle north1 = { 0.0f,0.0f,1.0f, 0.0f,1.0f,1.0f, 1.0f,1.0f,1.0f };
        triangle north2 = { 0.0f,0.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,0.0f,1.0f };

        triangle bottom1 = { 0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f,0.0f,1.0f };
        triangle bottom2 = { 0.0f,0.0f,0.0f, 1.0f,0.0f,1.0f, 1.0f,0.0f,0.0f };

        triangle extra = { 2.0f, 2.0f, 0.0f, 3.0f, 2.0f, 0.0f, 3.0f, 3.0f, 0.0f };

        cube.triangles = { south1, south2, east1, east2, west1, west2, top1, top2, north1, north2, bottom1, bottom2 };


        // Projection matrix
        float zNear = 0.1f;
        float zFar = 1000.0f;
        float fovDegrees= 90.0f;
        float aspectRatio = 0.5f;
        float inverseFovRad = 1.0f / tanf(fovDegrees * 0.5f / 180.0f * 3.14159f);

        projMatrix.matrix[0][0] = aspectRatio * inverseFovRad;
        projMatrix.matrix[1][1] = inverseFovRad;
        projMatrix.matrix[2][2] = zFar / (zFar - zNear);
        projMatrix.matrix[3][2] = (-zNear * zFar) / (zFar - zNear);
        projMatrix.matrix[2][3] = 1;
        projMatrix.matrix[3][3] = 0;

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override {

        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
        
        m4x4 matRotZ;
        m4x4 matRotX; 
        fTheta += 1.0f * fElapsedTime;

        // Rotation Z
        matRotZ.matrix[0][0] = cosf(fTheta);
        matRotZ.matrix[0][1] = sinf(fTheta);
        matRotZ.matrix[1][0] = -sinf(fTheta);
        matRotZ.matrix[1][1] = cosf(fTheta);
        matRotZ.matrix[2][2] = 1;
        matRotZ.matrix[3][3] = 1;

        // Rotation X
        matRotX.matrix[0][0] = 1;
        matRotX.matrix[1][1] = cosf(fTheta * 0.5f);
        matRotX.matrix[1][2] = sinf(fTheta * 0.5f);
        matRotX.matrix[2][1] = -sinf(fTheta * 0.5f);
        matRotX.matrix[2][2] = cosf(fTheta * 0.5f);
        matRotX.matrix[3][3] = 1;

        for (triangle tri : cube.triangles) {
            triangle triProjected;
            triangle triTranslated;
            triangle triRotatedZ;
            triangle triRotatedX;

            // Rotate Cube based on fTheta
            MultiplyMatrixVector(tri.points[0], triRotatedZ.points[0], matRotZ);
            MultiplyMatrixVector(tri.points[1], triRotatedZ.points[1], matRotZ);
            MultiplyMatrixVector(tri.points[2], triRotatedZ.points[2], matRotZ);

            MultiplyMatrixVector(triRotatedZ.points[0], triRotatedX.points[0], matRotX);
            MultiplyMatrixVector(triRotatedZ.points[1], triRotatedX.points[1], matRotX);
            MultiplyMatrixVector(triRotatedZ.points[2], triRotatedX.points[2], matRotX);

            // Translate cube backward
            triTranslated = triRotatedX;
            triTranslated.points[0].z += 2.0f;
            triTranslated.points[1].z += 2.0f;
            triTranslated.points[2].z += 2.0f;

            // Perspective Projection
            MultiplyMatrixVector(triTranslated.points[0], triProjected.points[0], projMatrix);
            MultiplyMatrixVector(triTranslated.points[1], triProjected.points[1], projMatrix);
            MultiplyMatrixVector(triTranslated.points[2], triProjected.points[2], projMatrix);

            // scale into view
            triProjected.points[0].x += 1.0f; triProjected.points[0].y += 1.0f;
            triProjected.points[1].x += 1.0f; triProjected.points[1].y += 1.0f;
            triProjected.points[2].x += 1.0f; triProjected.points[2].y += 1.0f;

            triProjected.points[0].x *= 0.5f * (float)ScreenWidth();
            triProjected.points[0].y *= 0.5f * (float)ScreenHeight();
            triProjected.points[1].x *= 0.5f * (float)ScreenWidth();
            triProjected.points[1].y *= 0.5f * (float)ScreenHeight();
            triProjected.points[2].x *= 0.5f * (float)ScreenWidth();
            triProjected.points[2].y *= 0.5f * (float)ScreenHeight();


            DrawTriangle(
                triProjected.points[0].x, triProjected.points[0].y,
                triProjected.points[1].x, triProjected.points[1].y,
                triProjected.points[2].x, triProjected.points[2].y,
                PIXEL_SOLID, FG_WHITE
            );
        }
        
        return true;
    }
};

int main()
{
    olcEngine3D demo;
    
    if (demo.ConstructConsole(140, 50, 1, 1)) {
        demo.Start();
    }


    return 0;
}