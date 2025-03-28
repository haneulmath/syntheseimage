#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include "glbasimac/glbi_set_of_points.hpp"
#include <iostream>
#include <cmath>

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;
static const float GL_VIEW_SIZE = 1.;
GLFWmonitor *usedMonitor = nullptr;
bool borderlessFullscreen = false;

double cusx, cusy;
bool manualColor = false;
unsigned short int r = 0, g = 0, b = 0;
int rRatio = 0, gRatio = 0, bRatio = 0;

// const GLFWvidmode* mode = glfwGetVideoMode(usedMonitor);
/* OpenGL Engine */
GLBI_Engine myEngine;
GLBI_Set_Of_Points thePoints;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, 1);
			break;

		case GLFW_KEY_A:
			borderlessFullscreen = !borderlessFullscreen;

			glfwSetWindowAttrib(window, GLFW_DECORATED, !borderlessFullscreen);
			glfwSetWindowAttrib(window, GLFW_FLOATING, borderlessFullscreen);

			if (!borderlessFullscreen)
			{
				glfwRestoreWindow(window);
			}
			else
			{
				glfwMaximizeWindow(window);
			}

			break;

		case GLFW_KEY_M:
			manualColor = !manualColor;
			break;
		case GLFW_KEY_R:
			rRatio = (mods == GLFW_MOD_SHIFT) ? -10 : 10;
			break;
		case GLFW_KEY_G:
			gRatio = (mods == GLFW_MOD_SHIFT) ? -10 : 10;

			break;
		case GLFW_KEY_B:
			bRatio = (mods == GLFW_MOD_SHIFT) ? -10 : 10;

			break;
		}
	}
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_R:
			rRatio = 0;

			break;
		case GLFW_KEY_G:
			gRatio = 0;
			break;
		case GLFW_KEY_B:
			bRatio = 0;
			break;
		}
	}
}

void onWindowResized(GLFWwindow * /*window*/, int width, int height)
{
	aspectRatio = width / (float)height;
	glViewport(0, 0, width, height);
	if (aspectRatio > 1.0)
	{
		myEngine.set2DProjection(-GL_VIEW_SIZE * aspectRatio / 2.,
								 GL_VIEW_SIZE * aspectRatio / 2.,
								 -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.);
	}
	else
	{
		myEngine.set2DProjection(-GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.,
								 -GL_VIEW_SIZE / (2. * aspectRatio),
								 GL_VIEW_SIZE / (2. * aspectRatio));
	}
}

/* Error handling function */
void onError(int error, const char *description)
{
	std::cout << "GLFW Error (" << error << ") : " << description << std::endl;
}

int main()
{


	// Initialize the library
	if (!glfwInit())
	{
		return -1;
	}

	/* Callback to a function if an error is rised by GLFW */
	glfwSetErrorCallback(onError);

	// Create a windowed mode window and its OpenGL context
	GLFWwindow *window = glfwCreateWindow(800, 800, "Ex3", usedMonitor, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	glfwSetWindowSizeCallback(window, onWindowResized);

	// Intialize glad (loads the OpenGL functions)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return -1;
	}

	// Initialize Rendering Engine
	myEngine.initGL();

	thePoints.initSet({0.f, 0.f, 0.5f, 0.f,  0.f, 0.5f}, {1, 1, 1, 1, 1, 1});

	onWindowResized(window, 800, 800);

	glfwSetKeyCallback(window, key_callback);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Get time (in second) at loop beginning */
		double startTime = glfwGetTime();

		/* Render here */

		// std::cout << cusx << std::endl;

		glfwGetCursorPos(window, &cusx, &cusy);

		// std::cout << cusx << std::endl;

		if (manualColor)
		{
			r += rRatio;
			r %= 256;
		}
		if (gRatio)
		{
			g += gRatio;
			g %= 256;
		}
		if (bRatio)
		{
			b += bRatio;
			b %= 256;
		}

		if (!manualColor)
		{
			glClearColor(std::fmod(std::abs(cusx), 256) / 256, std::fmod(std::abs(cusy), 256) / 256, 0, 0);
		}
		else
		{
			glClearColor(static_cast<float>(r) / 256, static_cast<float>(g) / 256, static_cast<float>(b) / 256, 0);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		// render here
		glPointSize(4.0);
		thePoints.drawSet();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		/* Elapsed time computation from loop begining */
		double elapsedTime = glfwGetTime() - startTime;
		/* If to few time is spend vs our wanted FPS, we wait */
		while (elapsedTime < FRAMERATE_IN_SECONDS)
		{
			glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS - elapsedTime);
			elapsedTime = glfwGetTime() - startTime;
		}
	}

	glfwTerminate();
	return 0;
}
