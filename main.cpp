#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "matrix.h"
#include "objhandler.h"
#include <algorithm>
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

    test = OBJHandler::loadFromObj("teapot.obj");
    // Utils::loadFromObj("VideoShip.obj", test);

    // Initialize Projection matrix
    Matrix mat(1.0f, 1000.0f, 90.0f, screenWidth, screenHeight);

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
            fYaw += 1.0f * timeElapsed;
        if(glfwGetKey(window, GLFW_KEY_RIGHT))
            fYaw -= 1.0f * timeElapsed;
        if(glfwGetKey(window, GLFW_KEY_UP))
            fPitch -= 1.0f * timeElapsed;
        if(glfwGetKey(window, GLFW_KEY_DOWN))
            fPitch += 1.0f * timeElapsed;

        

        /* Render here */
        
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);

        vector3d vUp = { 0.0f, 1.0f, 0.0f };
        vector3d vTarget = { 0.0f, 0.0f, 1.0f };
        mat4x4 matCameraRotY = Matrix::RotationMatrixY(fYaw);
        mat4x4 matCameraRotX = Matrix::RotationMatrixX(fPitch);
        mat4x4 matCameraRot = Matrix::MultiplyMatrixMatrix(matCameraRotX, matCameraRotY);
        
        vLookDir = Matrix::MultiplyVectorMatrix(vTarget, matCameraRot);
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
                
                // Scale into view
                // OPENGL DOES THIS BECAUSE ITS VIEW FRUSTUM IS FROM -1 to 1
                
                buffer.push_back(tri);
            }
        }

        // Sort triangles from back to front
        // technically we should sort triprojected's z vals but whatever
        sort(buffer.begin(), buffer.end(), [](triangle& t1, triangle& t2){
            float z_bar1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3;
            float z_bar2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3;
            return z_bar1>z_bar2;
        });

        for(triangle triTranslated : buffer) {
            
            triangle triView = Matrix::MultiplyTriangleMatrix(triTranslated, matView);

            // Projection matrix 3D -> 2D
            triangle triProjected = Matrix::MultiplyTriangleMatrix(triView, mat.getProjectionMatrix());
            // Divide each property by w after projection
            triProjected = { 
                Utils::vectorScale(triProjected.points[0], 1/triProjected.points[0].w), 
                Utils::vectorScale(triProjected.points[1], 1/triProjected.points[1].w),
                Utils::vectorScale(triProjected.points[2], 1/triProjected.points[2].w)
            };

            // Light triangle
            vector3d light_direction = {0.0f,0.0f,1.0f}; // Light is coming from 0,0,0
            light_direction = Utils::vectorFlip(light_direction);
            light_direction = Utils::normalize(light_direction);
            // Flip and normalize the vector

            vector3d surfaceNormal = Utils::surfaceNormal(triTranslated);
            float lit = Utils::dotProduct(surfaceNormal, light_direction);

            // We want the normal vector to face towards the light vector for full luminence
            glColor3f(1.0f,lit,lit);

            // Draw triangle
            glVertex2f(triProjected.points[0].x, triProjected.points[0].y);
            glVertex2f(triProjected.points[1].x, triProjected.points[1].y);
            glVertex2f(triProjected.points[2].x, triProjected.points[2].y);
        }

        glEnd();

        // /* Swap front and back buffers */
        glfwSwapBuffers(window);

        // /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}