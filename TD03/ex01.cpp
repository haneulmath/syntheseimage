#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include "glbasimac/glbi_set_of_points.hpp"
#include "glbasimac/glbi_convex_2D_shape.hpp"

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;

/* Animation parameters */
static float angle1 = 0.0f;  // Angle for the first arm rotation
static float angle2 = 0.0f;  // Angle for the second arm rotation
static float angle3 = 0.0f;  // Angle for the third arm (beater) rotation
static float animationSpeed = 1.0f;  // Speed of the animation

/* OpenGL Engine */
GLBI_Engine myEngine;
GLBI_Convex_2D_Shape carre;
GLBI_Convex_2D_Shape cercle;

// Déclarations anticipées des fonctions pour résoudre les dépendances cycliques
void drawFirstArm();
void drawSecondArm();
void drawThirdArm();
void drawRoundedSquare();

/**
 * Draws the main arm with two circles and a trapezoid
 */
void drawFirstArm() {
    // Save current matrix
    glPushMatrix();
    
    // Apply rotation around the origin (large circle center)
    glRotatef(angle1, 0.0f, 0.0f, 1.0f);
    
    // Draw the large circle (radius = 20)
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray
    glPushMatrix();
    glScalef(20.0f, 20.0f, 1.0f);
    cercle.drawShape();
    glPopMatrix();
    
    // Draw the trapezoid (connecting the two circles)
    glColor3f(0.7f, 0.7f, 0.7f); // Slightly darker gray
    glBegin(GL_QUADS);
        glVertex2f(-10.0f, -5.0f);   // Bottom left
        glVertex2f(50.0f, -5.0f);    // Bottom right
        glVertex2f(50.0f, 5.0f);     // Top right
        glVertex2f(-10.0f, 5.0f);    // Top left
    glEnd();
    
    // Move to the right to draw the small circle
    glTranslatef(50.0f, 0.0f, 0.0f);
    
    // Draw the small circle (radius = 10)
    glColor3f(0.85f, 0.85f, 0.85f); // Slightly lighter gray
    glPushMatrix();
    glScalef(10.0f, 10.0f, 1.0f);
    cercle.drawShape();
    glPopMatrix();
    
    // Draw the second arm from this pivot point
    drawSecondArm();
    
    // Restore matrix
    glPopMatrix();
}

/**
 * Draws a square with rounded corners
 */
void drawRoundedSquare() {
    // Using a simple square for now
    glPushMatrix();
    glScalef(5.0f, 5.0f, 1.0f);
    carre.drawShape();
    glPopMatrix();
}

/**
 * Draws the second arm
 */
void drawSecondArm() {
    glPushMatrix();
    
    // Apply rotation for the second arm
    glRotatef(angle2, 0.0f, 0.0f, 1.0f);
    
    // Draw the rectangular part of the arm
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, -3.0f);   // Bottom left
        glVertex2f(40.0f, -3.0f);  // Bottom right
        glVertex2f(40.0f, 3.0f);   // Top right
        glVertex2f(0.0f, 3.0f);    // Top left
    glEnd();
    
    // Draw the pivot on the left side (connection to first arm)
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glScalef(5.0f, 5.0f, 1.0f);
    cercle.drawShape();
    glPopMatrix();
    
    // Draw the pivot at the end of the arm (connection to third arm)
    glTranslatef(40.0f, 0.0f, 0.0f);
    glColor3f(0.75f, 0.75f, 0.75f);
    glPushMatrix();
    glScalef(5.0f, 5.0f, 1.0f);
    cercle.drawShape();
    glPopMatrix();
    
    // Draw the third arm from this pivot point
    drawThirdArm();
    
    glPopMatrix();
}

/**
 * Draws the beater/striker component
 */
void drawThirdArm() {
    glPushMatrix();
    
    // Apply rotation for the third arm (beater)
    glRotatef(angle3, 0.0f, 0.0f, 1.0f);
    
    // Draw the rod
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, -2.0f);   // Bottom left
        glVertex2f(35.0f, -2.0f);  // Bottom right
        glVertex2f(35.0f, 2.0f);   // Top right
        glVertex2f(0.0f, 2.0f);    // Top left
    glEnd();
    
    // Draw the pivot on the left side (connection to second arm)
    glColor3f(0.65f, 0.65f, 0.65f);
    glPushMatrix();
    glScalef(4.0f, 4.0f, 1.0f);
    cercle.drawShape();
    glPopMatrix();
    
    // Draw the ball at the end (the beater)
    glTranslatef(35.0f, 0.0f, 0.0f);
    glColor3f(0.9f, 0.3f, 0.3f); // Red for the beater
    glPushMatrix();
    glScalef(8.0f, 8.0f, 1.0f);
    cercle.drawShape();
    glPopMatrix();
    
    glPopMatrix();
}

/**
 * Draws the complete mechanical pile
 */
void drawCompletePile() {
    glPushMatrix();
    
    // Move the origin to lower part of the screen
    glTranslatef(0.0f, -30.0f, 0.0f);
    
    // Draw the base of the pile
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
        glVertex2f(-30.0f, -10.0f);
        glVertex2f(30.0f, -10.0f);
        glVertex2f(30.0f, 0.0f);
        glVertex2f(-30.0f, 0.0f);
    glEnd();
    
    // Draw the main arm assembly
    drawFirstArm();
    
    glPopMatrix();
}

/**
 * Updates the animation angles
 */
void updateAnimation() {
    // Update the angles for each arm
    angle1 += 0.5f * animationSpeed;
    angle2 = 30.0f * sin(glfwGetTime() * 1.5f * animationSpeed);
    angle3 = 45.0f * sin(glfwGetTime() * 2.0f * animationSpeed);
    
    // Make sure the angles stay within a reasonable range
    if (angle1 > 360.0f) {
        angle1 -= 360.0f;
    }
}

/* Error handling function */
void onError(int error, const char* description) {
    std::cout << "GLFW Error (" << error << ") : " << description << std::endl;
}

/**
 * Window resize callback
 */
void onWindowResized(GLFWwindow* /*window*/, int width, int height) {
    aspectRatio = width / (float) height;
    glViewport(0, 0, width, height);
}

/**
 * Key press callback
 */
void onKeyPressed(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    // Control animation speed
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        animationSpeed *= 1.2f; // Increase speed
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        animationSpeed *= 0.8f; // Decrease speed
    }
}

int main() {
    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }    

    /* Callback to a function if an error is raised by GLFW */
    glfwSetErrorCallback(onError);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1024, 720, "Pile Mécanique - TD03 Ex01", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Set the callback functions
    glfwSetWindowSizeCallback(window, onWindowResized);
    glfwSetKeyCallback(window, onKeyPressed);

    // Initialize glad (loads the OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    // Initialize Rendering Engine
    myEngine.initGL();
    
    // Initialize the square and circle shapes
    // Create a square (unit size)
    std::vector<float> squareVertices = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
    };
    carre.initShape(squareVertices);
    
    // Create a circle (unit radius)
    std::vector<float> circleVertices;
    const int numSegments = 32;
    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments;
        circleVertices.push_back(cos(angle));
        circleVertices.push_back(sin(angle));
    }
    cercle.initShape(circleVertices);
    
    // Print instructions
    std::cout << "===== Pile Mécanique - TD03 Ex01 =====" << std::endl;
    std::cout << "Utilisez les flèches HAUT/BAS pour changer la vitesse d'animation" << std::endl;
    std::cout << "Appuyez sur ECHAP pour quitter" << std::endl;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Get time (in second) at loop beginning */
        double startTime = glfwGetTime();

        /* Render here */
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Set up the view with proper aspect ratio
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (aspectRatio > 1.0f) {
            glOrtho(-100.0 * aspectRatio, 100.0 * aspectRatio, -100.0, 100.0, -1.0, 1.0);
        } else {
            glOrtho(-100.0, 100.0, -100.0 / aspectRatio, 100.0 / aspectRatio, -1.0, 1.0);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // Update animation and draw the complete pile
        updateAnimation();
        drawCompletePile();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        /* Elapsed time computation from loop beginning */
        double elapsedTime = glfwGetTime() - startTime;
        /* If too little time is spent vs our wanted FPS, we wait */
        while(elapsedTime < FRAMERATE_IN_SECONDS) {
            glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS-elapsedTime);
            elapsedTime = glfwGetTime() - startTime;
        }
    }

    glfwTerminate();
    return 0;
}