#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "matrix.h"
#include <algorithm>
using namespace std;
int main()
{
    // Initialize variables
    double prev = 0;
    double curr;

    GLFWwindow* window;

    int screenWidth = 640;
    int screenHeight = 480;

    mesh cube;

    cube.triangles = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	};

    mesh ship; 
    Utils::loadFromObj("VideoShip.obj", ship);

    // Initialize Projection matrix
    Matrix mat(0.1f, 1000.0f, 90.0f, screenWidth, screenHeight);

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

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */

    int fps = 0;
    while (!glfwWindowShouldClose(window)) {
        float fTheta = glfwGetTime();
        
        //Show fps in window
        fps++;
        curr = fTheta;
        if(curr - prev >= 1.0f) {
            prev = curr;
            std::string text = "FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, text.c_str());
            fps = 0;
        }  

        // Calculate rotation matrix

        // Rotation matrix Z
        mat4x4 matRotZ = Matrix::RotationMatrixZ(fTheta);

        // Rotation X
        mat4x4 matRotX = Matrix::RotationMatrixX(fTheta * 0.5);

        /* Render here */
        
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);

        // Holds the projected and translated triangles
        vector<triangle> buffer;

        for(triangle tri : ship.triangles) {

            triangle triTranslated;
            triangle triRotatedZ;
            triangle triRotatedX;

            // Rotate Cube based on fTheta
            Matrix::MultiplyMatrixTriangle(tri, triRotatedZ, matRotZ);
            Matrix::MultiplyMatrixTriangle(triRotatedZ, triRotatedX, matRotX);

            // push cube back a bit
            Matrix::TranslateTriangle(triRotatedX, triTranslated, {0.0f, 0.0f, 10.0f});


            // Queue triangle to render if it is visible from camera
            vector3d vCamera = {0.0f,0.0f,0.0f};
            vector3d linefromcameratotriangle = {triTranslated.points[0].x - vCamera.x, 
            triTranslated.points[0].y - vCamera.y, triTranslated.points[0].z - vCamera.z};
            // Find if it is visible by comparing the vector from camera to the surface normal vector of the trianlge
            if(Utils::dotProduct(linefromcameratotriangle, Utils::surfaceNormal(triTranslated)) < 0.0f) {
                
                // Scale into view
                // OPENGL DOES THIS BECAUSE ITS VIEW FRUSTUM IS FROM -1 to 1
                
                buffer.push_back(triTranslated);
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

            triangle triProjected;
            // Projection matrix 3D -> 2D
            Matrix::MultiplyMatrixTriangle(triTranslated, triProjected, mat.getProjectionMatrix());

            // Light triangle
            vector3d light_direction = {0.0f,0.0f,1.0f}; // Light is coming from player
            Utils::flip(light_direction, light_direction);
            Utils::normalize(light_direction, light_direction);
            float lit = Utils::dotProduct(Utils::surfaceNormal(triTranslated), light_direction); 
            // We want the normal vector to face towards the light vector for full luminence
            glColor3f(lit,lit,lit);
            
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