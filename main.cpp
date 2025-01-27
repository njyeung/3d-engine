#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "matrix.h"
#include "objhandler.h"
#include <algorithm>
 #define STB_IMAGE_IMPLEMENTATION
#include "include/stb/stb_image.h"
using namespace std;

mesh test;
vector3d vCamera = { 0.0f, 0.0f, -10.0f};
vector3d vLookDir;
float fYaw;
float fPitch;
float currentFrameTime = glfwGetTime();
float previousFrameTime;
float timeElapsed;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
    // Initialize variables
    double prev = 0;
    double curr;

    GLFWwindow* window;

    int screenWidth = 840;
    int screenHeight = 680;

    test = OBJHandler::loadFromObj("Artisans Hub.obj");
    // test = OBJHandler::loadFromObj("mountains.obj");

    // Initialize Projection matrix
    Matrix mat(1.0f, 1000.0f, 90.0f, screenWidth, screenHeight);
    
    // World lighting
    vector3d light_direction = {0.0f,0.0f,1.0f}; // Light is coming from 0,0,0
    light_direction = Utils::vectorFlip(light_direction);
    light_direction = Utils::normalize(light_direction);
    // Flip and normalize the vector

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "FPS: 0", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    int widthImg, heightImg, numColCh;
    unsigned char* bytes = stbi_load("High.png", &widthImg, &heightImg, &numColCh, 0);

    glEnable(GL_TEXTURE_2D);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0,GL_RGB,GL_UNSIGNED_BYTE, bytes);

    /* Loop until the user closes the window */

    int fps = 0;

    while (!glfwWindowShouldClose(window)) {
        previousFrameTime = currentFrameTime;
        currentFrameTime = glfwGetTime();
        timeElapsed = currentFrameTime - previousFrameTime;

        //Show fps in window
        fps++;
        curr = currentFrameTime;
        if(curr - prev >= 1.0f) {
            prev = curr;
            std::string text = "FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, text.c_str());
            fps = 0;
        }  

        if(glfwGetKey(window, GLFW_KEY_SPACE))
            vCamera.y += 5.0f * timeElapsed;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
            vCamera.y -= 5.0f * timeElapsed;
        vector3d vForward = Utils::vectorScale(vLookDir, 5.0f * timeElapsed);
        vector3d vLeft = Utils::vectorScale(Utils::normalize(Utils::crossProduct(vLookDir, {0.0f, 0.1f, 0.0f})), 5.0f * timeElapsed);
        if(glfwGetKey(window, GLFW_KEY_W))
            vCamera = Utils::vectorAdd(vCamera, vForward);
        if(glfwGetKey(window, GLFW_KEY_S))
            vCamera = Utils::vectorSubtract(vCamera, vForward);
        if(glfwGetKey(window, GLFW_KEY_A))
            vCamera = Utils::vectorAdd(vCamera, vLeft);
        if(glfwGetKey(window, GLFW_KEY_D))
            vCamera = Utils::vectorSubtract(vCamera, vLeft);
        
        if(glfwGetKey(window, GLFW_KEY_LEFT))
            fYaw += 2.0f * timeElapsed;
        if(glfwGetKey(window, GLFW_KEY_RIGHT))
            fYaw -= 2.0f * timeElapsed;
        if(glfwGetKey(window, GLFW_KEY_UP))
            fPitch -= 2.0f * timeElapsed;
        if(glfwGetKey(window, GLFW_KEY_DOWN))
            fPitch += 2.0f * timeElapsed;


        /* Render here */
        
        glClear(GL_COLOR_BUFFER_BIT);

        vector3d vUp = { 0.0f, 1.0f, 0.0f };
        vector3d vTarget = { 0.0f, 0.0f, 1.0f };

        mat4x4 matCameraRotY = Matrix::RotationMatrixY(fYaw);
        vLookDir = Matrix::MultiplyVectorMatrix(vTarget, matCameraRotY);
        
        // // Recalculate local X-axis
        // vector3d vRight = Utils::crossProduct(vUp, vLookDir);
        // vRight = Utils::normalize(vRight);
        
        // // Apply pitch around local axis
        // mat4x4 matCameraRotX = Matrix::RotationMatrixAxis(fPitch, vRight);
        
        // vLookDir = Matrix::MultiplyVectorMatrix(vLookDir, matCameraRotX);

        // // Recalculate the up vector to ensure orthogonality
        // vUp = Utils::crossProduct(vLookDir, vRight);
        // vUp = Utils::normalize(vUp);

        vTarget = Utils::vectorAdd(vCamera, vLookDir);
        mat4x4 matCamera = Matrix::PointAtMatrix(vCamera, vTarget, vUp);
        // Invert camera matrix to transform everything else around the camera
        mat4x4 matView = Matrix::QuickInverseMatrix(matCamera);

        // Holds the projected and translated triangles
        vector<triangle> buffer;

        for(triangle tri : test.triangles) {
            
            // Queue triangle to render if it is visible from camera
            vector3d vCameraRay = Utils::vectorSubtract(tri.points[0], vCamera);
            vector3d surfaceNormal = Utils::surfaceNormal(tri);
            
            // Find if it is visible by comparing the vector from camera to the surface normal vector of the trianlge
            if(Utils::dotProduct(vCameraRay, surfaceNormal) < 0.0f) { 
                
                // Light triangle
                vector3d surfaceNormal = Utils::surfaceNormal(tri);
                tri.color = Utils::dotProduct(surfaceNormal, light_direction);

                // Apply translations and rotations based on camera location
                triangle triView = Matrix::MultiplyTriangleMatrix(tri, matView);

                buffer.push_back(triView);
            }
        }

        // Sort triangles from back to front
        // technically we should sort triprojected's z vals but whatever
        sort(buffer.begin(), buffer.end(), [](triangle& t1, triangle& t2){
            float z_bar1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3;
            float z_bar2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3;
            return z_bar1>z_bar2;
        });

        for(triangle tri : buffer) {
            
            triangle clipped[2];
            clipped[0].color = tri.color;
            clipped[1].color = tri.color;
            int numClippedTriangles = Utils::clipTriangleAgainstPlane({0.0f,0.0f,mat.getFNear()}, {0.0f,0.0f,1.0f}, tri, clipped[0], clipped[1]);

            for(int i = 0; i<numClippedTriangles; i++) {
                
                // Projection matrix 3D -> 2D
                triangle triProjected = Matrix::MultiplyTriangleMatrix(clipped[i], mat.getProjectionMatrix());
                
                // This fix is for opengl's right handed coordinate system
                // triProjected.points[0].x = 1-triProjected.points[0].x;
                // triProjected.points[1].x = 1-triProjected.points[1].x;
                // triProjected.points[2].x = 1-triProjected.points[2].x;

                // Divide each property by w after projection
                triProjected.points[0] = Utils::vectorScale(triProjected.points[0], 1/triProjected.points[0].w);
                triProjected.points[1] = Utils::vectorScale(triProjected.points[1], 1/triProjected.points[1].w);
                triProjected.points[2] = Utils::vectorScale(triProjected.points[2], 1/triProjected.points[2].w);
                

                float color = max(triProjected.color, 0.3f);
                glColor3f(1.0f, 1.0f,  1.0f);
                glBegin(GL_TRIANGLE_FAN);
                
                // Draw triangle
                glTexCoord2f(triProjected.texture[0].u, triProjected.texture[0].v);
                glVertex2f(triProjected.points[0].x, triProjected.points[0].y);
                
                glTexCoord2f(triProjected.texture[1].u, triProjected.texture[1].v);
                glVertex2f(triProjected.points[1].x, triProjected.points[1].y);
                
                glTexCoord2f(triProjected.texture[2].u, triProjected.texture[2].v);
                glVertex2f(triProjected.points[2].x, triProjected.points[2].y);
                
                glEnd();
            }
        }

        // glEnd();

        // /* Swap front and back buffers */
        glfwSwapBuffers(window);

        // /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}