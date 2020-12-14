#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <stdlib.h>
#include <time.h>

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
	static Geometry* lobby;
	static Transform* playerAstroMoveControl;
	static Transform* playerAstroFaceControl;
	static std::vector<Transform*> computerAstroMoveList;
	static std::vector<Transform*> computerAstroFaceList;
	static std::vector<float> angleList;
	static std::vector<int> colorIndexList;

	// Key Transform node that control animation
	// Camera Matrices
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eyePos, lookAtPoint, upVector;

	// Light Source
	static glm::vec3 lightPos;
	static glm::vec3 lightColor;

	// astro color list and count
	static std::vector<glm::vec3> colorList;
	static std::vector<bool> colorStatus;

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
	static void playerMovement();
	static void computerMovement();

	static glm::vec2 pressedPos;
	static glm::vec3 prevPoint;
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static glm::vec3 trackBallMapping(glm::vec2 point);

	// collision detection
	static float lobbyCollide(glm::vec3 location, float angle);
	static float astroCollide(glm::vec3 location, float angle);

	// randomly add astro
	static void randomAdd();
	// randomly remove astro
	static void randomRemove();

	static void randomToggle();
};

#endif
