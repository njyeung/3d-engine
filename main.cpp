#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "matrix.h"
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

    // Initialize Projection matrix
    Matrix m(0.1f, 1000.0f, 90.0f, screenWidth, screenHeight);

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
        mat4x4 matRotZ;
        matRotZ.matrix[0][0] = cosf(fTheta);
        matRotZ.matrix[0][1] = sinf(fTheta);
        matRotZ.matrix[1][0] = -sinf(fTheta);
        matRotZ.matrix[1][1] = cosf(fTheta);
        matRotZ.matrix[2][2] = 1;
        matRotZ.matrix[3][3] = 1;

        // Rotation X
        mat4x4 matRotX;
        matRotX.matrix[0][0] = 1;
        matRotX.matrix[1][1] = cosf(fTheta * 0.5f);
        matRotX.matrix[1][2] = sinf(fTheta * 0.5f);
        matRotX.matrix[2][1] = -sinf(fTheta * 0.5f);
        matRotX.matrix[2][2] = cosf(fTheta * 0.5f);
        matRotX.matrix[3][3] = 1;

        /* Render here */
        
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);

        for(triangle tri : cube.triangles) {

            triangle triProjected;
            triangle triTranslated;
            triangle triRotatedZ;
            triangle triRotatedX;

            // Rotate Cube based on fTheta
            Matrix::MultiplyMatrixTriangle(tri, triRotatedZ, matRotZ);
            Matrix::MultiplyMatrixTriangle(triRotatedZ, triRotatedX, matRotX);

            // push cube back a bit
            Matrix::TranslateTriangle(triRotatedX, triTranslated, {0.0f, 0.0f, 3.0f});

            // // Projection matrix
            Matrix::MultiplyMatrixTriangle(triTranslated, triProjected, m.projectionMatrix);

            // Scale into view
            // OPENGL DOES THIS BECAUSE ITS VIEW FRUSTUM IS FROM -1 to 1

            // Draw triangle
            glColor3f(1.0f,1.0f,1.0f);
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