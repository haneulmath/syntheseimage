#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include "glbasimac/glbi_set_of_points.hpp"
#include "glbasimac/glbi_convex_2D_shape.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;
static const float GL_VIEW_SIZE = 6.0f; // Taille de la vue OpenGL modifiée à 6 (de -3 à 3 sur chaque axe)

/* OpenGL Engine */
GLBI_Engine myEngine;
GLBI_Set_Of_Points thePoints; // Correction de la déclaration de thePoints
GLBI_Convex_2D_Shape carre;
GLBI_Convex_2D_Shape triangle;
GLBI_Convex_2D_Shape cercle;

// Variable pour contrôler quelle forme est affichée
int currentShape = 0; // 0 = carré, 1 = triangle, 2 = cercle

/* Error handling function */
void onError(int error, const char* description) {
	std::cout << "GLFW Error ("<<error<<") : " << description << std::endl;
}

// Callback pour gérer les touches du clavier
void onKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_SPACE) {
            // Basculer entre les formes avec la touche espace
            currentShape = (currentShape + 1) % 3;
            std::cout << "Forme actuelle: " << (currentShape == 0 ? "Carré" : (currentShape == 1 ? "Triangle" : "Cercle")) << std::endl;
        }
    }
}

// Callback pour le redimensionnement de la fenêtre
void onWindowResized(GLFWwindow* window, int width, int height) {
    aspectRatio = width / (float) height;
    std::cout << "Window resized: " << width << "x" << height << ", aspectRatio = " << aspectRatio << std::endl;
}

// Callback pour les clics de souris
void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Récupérer les coordonnées du curseur
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Récupérer les dimensions de la fenêtre
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        // Conversion des coordonnées de l'écran vers l'espace virtuel
        float virtualX, virtualY;
        
        if (aspectRatio >= 1.0f) { // Format paysage ou carré
            virtualX = ((xpos / width) * 2.0f - 1.0f) * aspectRatio;
            virtualY = -((ypos / height) * 2.0f - 1.0f); // Inversion de l'axe Y
        } else { // Format portrait
            virtualX = ((xpos / width) * 2.0f - 1.0f);
            virtualY = -((ypos / height) * 2.0f - 1.0f) / aspectRatio; // Inversion de l'axe Y
        }
        
        std::cout << "Clic à la position écran: (" << xpos << ", " << ypos 
                  << ") -> position virtuelle: (" << virtualX << ", " << virtualY << ")" << std::endl;
        
        // Créer les tableaux pour les coordonnées et la couleur
        float coords[2] = {virtualX, virtualY};
        float color[3] = {1.0f, 1.0f, 1.0f}; // Point blanc par défaut
        
        // Ajouter le point à notre set de points
        thePoints.addAPoint(coords, color);
    }
}

// Fonction d'initialisation de la scène
void initScene() {
    // === CARRÉ ===
    // Initialiser un carré unitaire (longueur des côtés = 1) centré sur l'origine
    std::vector<float> pointsCarre = {
        -0.5f, -0.5f,  // Coin inférieur gauche
        0.5f, -0.5f,   // Coin inférieur droit
        0.5f, 0.5f,    // Coin supérieur droit
        -0.5f, 0.5f    // Coin supérieur gauche
    };
    carre.initShape(pointsCarre);
    
    // === TRIANGLE ===
    // Initialiser un triangle avec les coordonnées données
    std::vector<float> pointsTriangle = {
        -0.5f, -0.5f,  // Premier point
        0.5f, -0.5f,   // Deuxième point
        0.0f, 0.5f     // Troisième point
    };
    triangle.initShape(pointsTriangle);
    
    // === CERCLE ===
    // Initialiser un cercle unitaire (rayon = 0.5)
    std::vector<float> pointsCercle;
    const int segments = 36; // Nombre de segments pour approximer le cercle
    const float radius = 0.5f;
    
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        pointsCercle.push_back(x);
        pointsCercle.push_back(y);
    }
    cercle.initShape(pointsCercle);
    
    // Changer le mode de rendu pour permettre le remplissage
    carre.changeNature(GL_POLYGON);  // GL_LINE_LOOP pour contour, GL_POLYGON pour remplissage
    triangle.changeNature(GL_POLYGON);
    cercle.changeNature(GL_POLYGON);
    
    // Initialisation des points (code existant)
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

    // Initialiser les points avec leurs couleurs respectives
	thePoints.initSet(points, colors);
}

// Fonction de rendu de la scène
void renderScene() {
    // Appliquer la taille virtuelle de la fenêtre (GL_VIEW_SIZE)
    if (aspectRatio >= 1.0f) {
        myEngine.setViewport(-GL_VIEW_SIZE/2.0f * aspectRatio, GL_VIEW_SIZE/2.0f * aspectRatio, -GL_VIEW_SIZE/2.0f, GL_VIEW_SIZE/2.0f);
    } else {
        myEngine.setViewport(-GL_VIEW_SIZE/2.0f, GL_VIEW_SIZE/2.0f, -GL_VIEW_SIZE/2.0f / aspectRatio, GL_VIEW_SIZE/2.0f / aspectRatio);
    }

    // === ÉTAPE 1: Dessiner un disque (cercle plein) de diamètre 1.0 au centre ===
    myEngine.setIdentityMatrix(); // Matrice identité initiale
    myEngine.setFlatColor(0.8f, 0.5f, 0.2f); // Couleur orange-brun pour le chat
    cercle.drawShape();
    
    // === ÉTAPE 2: Décaler le disque de 1.0 sur la droite (x positif) ===
    myEngine.setIdentityMatrix(); // Réinitialiser la matrice
    myEngine.translateMatrix(1.0f, 0.0f); // Décalage de 1.0 sur l'axe x
    myEngine.setFlatColor(0.8f, 0.5f, 0.2f);
    cercle.drawShape();
    myEngine.setIdentityMatrix(); // Recharger la matrice identité après le rendu
    
    // === ÉTAPE 3: Enlever le décalage et dessiner les oreilles ===
    // Retour au cercle centré (tête du chat)
    myEngine.setIdentityMatrix();
    myEngine.setFlatColor(0.8f, 0.5f, 0.2f);
    cercle.drawShape();
    
    // === ÉTAPE 4: Dessiner l'oreille droite ===
    myEngine.setIdentityMatrix(); // Réinitialiser la matrice
    
    // 1. Translater à 45 degrés (Pi/4) en haut à droite du disque
    float angle = M_PI / 4.0f; // 45 degrés en radians
    float distance = 0.5f; // Moitié du diamètre du cercle
    myEngine.translateMatrix(distance * cos(angle), distance * sin(angle));
    
    // 2. Rotation de 60 degrés (Pi/3)
    myEngine.rotateMatrix(M_PI / 3.0f);
    
    // 3. Homothétie de 0.5 sur les axes x et y
    myEngine.scaleMatrix(0.5f, 0.5f);
    
    // Dessiner l'oreille droite (triangle)
    myEngine.setFlatColor(0.7f, 0.4f, 0.1f); // Légèrement plus foncé pour les oreilles
    triangle.drawShape();
    
    // === ÉTAPE 5: Dessiner l'oreille gauche ===
    myEngine.setIdentityMatrix(); // Réinitialiser la matrice
    
    // Mêmes transformations mais avec angle opposé pour l'oreille gauche
    angle = M_PI - (M_PI / 4.0f); // 135 degrés en radians (symétrique par rapport à l'axe y)
    myEngine.translateMatrix(distance * cos(angle), distance * sin(angle));
    myEngine.rotateMatrix(-M_PI / 3.0f); // Rotation dans le sens opposé
    myEngine.scaleMatrix(0.5f, 0.5f);
    
    // Dessiner l'oreille gauche (même triangle)
    myEngine.setFlatColor(0.7f, 0.4f, 0.1f);
    triangle.drawShape();
    
    // === ÉTAPE 6: Dessiner les yeux du chat (en noir) ===
    myEngine.setIdentityMatrix();
    myEngine.translateMatrix(-0.2f, 0.1f); // Position de l'œil gauche
    myEngine.scaleMatrix(0.15f, 0.15f);    // Petite taille pour les yeux
    myEngine.setFlatColor(0.0f, 0.0f, 0.0f); // Noir pour les yeux
    cercle.drawShape();
    
    myEngine.setIdentityMatrix();
    myEngine.translateMatrix(0.2f, 0.1f);   // Position de l'œil droit
    myEngine.scaleMatrix(0.15f, 0.15f);     // Petite taille pour les yeux
    myEngine.setFlatColor(0.0f, 0.0f, 0.0f); // Noir pour les yeux
    cercle.drawShape();
    
    // Réinitialiser la matrice identité à la fin
    myEngine.setIdentityMatrix();
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

    // Définir les callbacks
    glfwSetFramebufferSizeCallback(window, onWindowResized);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetKeyCallback(window, onKeyboard);
	
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
