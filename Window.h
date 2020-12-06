#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "shader.h"
#include "Object.h"
#include "Cube.h"
#include "Skybox.h"
#include "Sphere.h"
#include "Transform.h"
#include "Geometry.h"
#include "SphereNode.h"

struct KeyRecord {
	bool wPressed;
	bool aPressed;
	bool sPressed;
	bool dPressed;
	bool pPressed;
	bool mousePressed;
};

class Window
{
public:

	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;

	// Root of scene graph, world
	static Transform* world;
	static Transform* astroMoveControl;
	static Transform* astroFaceControl;

	// Key Transform node that control animation
	// Camera Matrices
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eyePos, lookAtPoint, upVector;

	// Light Source
	static glm::vec3 lightPos;
	static glm::vec3 lightColor;

	// Shader Program ID
	static GLuint phongShader;
	static GLuint toonShader;

	// Constructors and Destructors
	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();

	// Window functions
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	// Draw and Update functions
	static void idleCallback();
	static void displayCallback(GLFWwindow*);

	// Callbacks
	static float speed;
	static KeyRecord keyPressed;
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void movement();

	static glm::vec2 pressedPos;
	static glm::vec3 prevPoint;
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static glm::vec3 trackBallMapping(glm::vec2 point);

	static bool lobbyCollide(glm::vec3 location);
};

#endif
