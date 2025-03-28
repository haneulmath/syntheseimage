#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include <iostream>

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;

/* OpenGL Engine */
GLBI_Engine myEngine;

/* Background color */
static float bgColor[3] = {0.2f, 0.0f, 0.0f};

/* Key states */
static bool increaseRed = false;
static bool increaseGreen = false;
static bool increaseBlue = false;
static bool shiftPressed = false;

/* Error handling function */
void onError(int error, const char* description) {
    std::cout << "GLFW Error ("<<error<<") : " << description << std::endl;
}

/* Espace virtuel */
static const float GL_VIEW_SIZE = 1.;
// L'univers 2D visible a une taille de 1.0 en x et en y

void onWindowResized(GLFWwindow* /*window*/, int width, int height)
{
    aspectRatio = width / (float) height;
    glViewport(0, 0, width, height);
    if( aspectRatio > 1.0)
    {
        myEngine.set2DProjection(-GL_VIEW_SIZE * aspectRatio/ 2.,
            GL_VIEW_SIZE * aspectRatio / 2. ,
            -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.);
    }
    else
    {
        myEngine.set2DProjection(-GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.,
        -GL_VIEW_SIZE / (2. * aspectRatio), 
        GL_VIEW_SIZE / (2. * aspectRatio));
    }
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    onWindowResized(window, width, height);
    glfwGetWindowSize(window, &width, &height);
}

// Modifier couleur avec les touches du clavier
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_R) {
        if (action == GLFW_PRESS) increaseRed = true;
        if (action == GLFW_RELEASE) increaseRed = false;
    }

    if (key == GLFW_KEY_G) {
        if (action == GLFW_PRESS) increaseGreen = true;
        if (action == GLFW_RELEASE) increaseGreen = false;
    }

    if (key == GLFW_KEY_B) {
        if (action == GLFW_PRESS) increaseBlue = true;
        if (action == GLFW_RELEASE) increaseBlue = false;
    }

    if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
        if (action == GLFW_PRESS) shiftPressed = true;
        if (action == GLFW_RELEASE) shiftPressed = false;
    }
}

// Modifier couleur avec la position du clic du curseur
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Convertir les coordonnées de la souris en coordonnées de fenêtre
        ypos = height - ypos;

        // Mettre à jour la couleur de fond
        bgColor[0] = (float)((int)xpos % 256) / 255.0f;
        bgColor[1] = (float)((int)ypos % 256) / 255.0f;
        bgColor[1] = (float)((int)ypos % 256) / 255.0f;
    }
}

int main() {
    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    /* Callback to a function if an error is rised by GLFW */
    glfwSetErrorCallback(onError);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 800, "TD 01 Ex 04", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Ajout de l'appel à la fonction glfwSetWindowSizeCallback
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Ajout de l'appel à la fonction glfwSetKeyCallback
    glfwSetKeyCallback(window, key_callback);

    // Ajout de l'appel à la fonction glfwSetMouseButtonCallback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Intialize glad (loads the OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    // Initialize Rendering Engine
    myEngine.initGL();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Get time (in second) at loop beginning */
        double startTime = glfwGetTime();

        /* Update background color based on key states */
        if (shiftPressed) {
            if (increaseRed && bgColor[0] > 0.0f) bgColor[0] -= 0.1f;
            if (increaseGreen && bgColor[1] > 0.0f) bgColor[1] -= 0.1f;
            if (increaseBlue && bgColor[2] > 0.0f) bgColor[2] -= 0.1f;
        } else {
            if (increaseRed && bgColor[0] < 1.0f) bgColor[0] += 0.1f;
            if (increaseGreen && bgColor[1] < 1.0f) bgColor[1] += 0.1f;
            if (increaseBlue && bgColor[2] < 1.0f) bgColor[2] += 0.1f;
        }

        /* Render here */
        glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render here

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        /* Elapsed time computation from loop begining */
        double elapsedTime = glfwGetTime() - startTime;
        /* If to few time is spend vs our wanted FPS, we wait */
        while(elapsedTime < FRAMERATE_IN_SECONDS)
        {
            glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS-elapsedTime);
            elapsedTime = glfwGetTime() - startTime;
        }
    }

    glfwTerminate();
    return 0;
}