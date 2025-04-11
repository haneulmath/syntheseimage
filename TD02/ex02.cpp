#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include "glbasimac/glbi_set_of_points.hpp"
#include <iostream>
#include <vector>

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;
static const float GL_VIEW_SIZE = 2.0f; // Taille de la vue OpenGL (-1 à 1 sur chaque axe)
static bool isLineMode = false; // Mode d'affichage (points ou ligne)

/* OpenGL Engine */
GLBI_Engine myEngine;
GLBI_Set_Of_Points thePoints; // Correction de la déclaration de thePoints

/* Error handling function */
void onError(int error, const char* description) {
	std::cout << "GLFW Error ("<<error<<") : " << description << std::endl;
}

// Callback pour le clavier
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        // Basculer entre le mode points et le mode ligne
        isLineMode = !isLineMode;
        
        if (isLineMode) {
            // Passer en mode ligne
            thePoints.changeNature(GL_LINE_STRIP);
            std::cout << "Mode d'affichage: Ligne (GL_LINE_STRIP)" << std::endl;
        } else {
            // Revenir en mode points
            thePoints.changeNature(GL_POINTS);
            std::cout << "Mode d'affichage: Points (GL_POINTS)" << std::endl;
        }
    }
}

// Fonction d'initialisation de la scène
void initScene() {
    // Créer un vector de float contenant les coordonnées des points
    std::vector<float> points = {
		0.0f, 0.0f,   // Point à l'origine
        0.5f, 0.0f,   // Point à droite
        0.0f, 0.5f,   // Point en haut
        -0.5f, -0.5f  // Point en bas à gauche
    };

    // Créer un vector de float contenant les couleurs des points (r, g, b pour chaque point)
    std::vector<float> colors = {
        1.0f, 1.0f, 1.0f,  // Blanc pour le point d'origine
        1.0f, 0.0f, 0.0f,  // Rouge pour le point à droite
        0.0f, 1.0f, 0.0f,  // Vert pour le point en haut
        0.8f, 0.0f, 0.8f   // Violet pour le point en bas à gauche
    };

    // Afficher les informations sur les points dans la console
    std::cout << "Nombre de points initialisés: " << points.size() / 2 << std::endl;
    for (size_t i = 0; i < points.size() / 2; i++) {
        std::cout << "Point " << i << ": (" << points[i*2] << ", " << points[i*2+1] 
                  << ") - Couleur: (" << colors[i*3] << ", " << colors[i*3+1] << ", " << colors[i*3+2] << ")" << std::endl;
    }

    // Initialiser les points avec leurs couleurs respectives
	thePoints.initSet(points, colors);
}

// Fonction de rendu de la scène
void renderScene() {
    // Définir la taille du point
    glPointSize(10.0);  // Augmentation de la taille des points

    // Dessiner les points
    thePoints.drawSet();
}

int main() {
    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }	

    /* Callback to a function if an error is rised by GLFW */
	glfwSetErrorCallback(onError);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1024, 720, "OpenGLTemplate", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

	// Intialize glad (loads the OpenGL functions)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	// Initialize Rendering Engine
	myEngine.initGL();

    // Initialiser la scène
    initScene();

    // Définir le callback pour le clavier
    glfwSetKeyCallback(window, onKeyPressed);
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Get time (in second) at loop beginning */
		double startTime = glfwGetTime();

		/* Render here */
		glClearColor(0.2f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);

        // Rendu de la scène
        renderScene();

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
