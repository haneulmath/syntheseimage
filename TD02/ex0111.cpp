#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include "glbasimac/glbi_set_of_points.hpp"
#include <iostream>
#include <vector> // Ajout pour utiliser std::vector

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;

/* OpenGL Engine */
GLBI_Engine myEngine;
GLBI_Set_Of_Points thePoints;

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
	
}

int main() {
    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    /* Callback to a function if an error is rised by GLFW */
	glfwSetErrorCallback(onError);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 800, "TD 01 Ex 03", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

	glfwSetWindowSizeCallback(window, window_size_callback);

    // Make the window's context current
    glfwMakeContextCurrent(window);

	// Intialize glad (loads the OpenGL functions)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	// Initialize Rendering Engine
	myEngine.initGL();

    // Création d'un vecteur contenant les coordonnées du point origine
    std::vector<float> originPoint = {0.0f, 0.0f};

    // Initialisation d'un point blanc à l'origine
    // thePoints.initSet(originPoint[0], originPoint[1], 1.0f, 1.0f, 1.0f); 
	thePoints.initSet(originPoint, 1.0f, 1.0f, 1.0f);

    // Création d'un vecteur contenant les coordonnées des points supplémentaires
    std::vector<std::vector<float>> additionalPoints = {
        {0.5f, 0.0f},
        {0.0f, 0.5f},
        {-0.5f, -0.5f}
    };

    // Ajout des points blancs aux coordonnées spécifiées
    for (const auto& point : additionalPoints) {
        thePoints.addPoint(point, 1.0f, 1.0f, 1.0f); // Couleur blanche (R=1, G=1, B=1)
    }

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Get time (in second) at loop beginning */
		double startTime = glfwGetTime();

		/* Render here */
		glClearColor(0.2f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);

        // Définir la taille des points
        glPointSize(4.0);

        // Dessiner les points
        thePoints.drawSet();

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