#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "GLFW Starter Project";

// Objects to Render
//Sphere* Window::disco;
Transform* Window::world;
Geometry* Window::lobby;
Transform* Window::playerAstroMoveControl;
Transform* Window::playerAstroFaceControl;
std::vector<Transform*> Window::computerAstroMoveList;
std::vector<Transform*> Window::computerAstroFaceList;
std::vector<Particle*> Window::particleList;
std::vector<float> Window::angleList;
std::vector<int> Window::colorIndexList;

int Window::removeDelay = 200;
int Window::indexToRemove = -1;

// Track key pressed
KeyRecord Window::keyPressed;

// Camera Matrices 
// Projection matrix:
glm::mat4 Window::projection; 

// View Matrix:
glm::vec3 Window::eyePos(0, 25, 25);			// Camera position.
glm::vec3 Window::lookAtPoint(0, 0, 0);		// The point we are looking at.
glm::vec3 Window::upVector(glm::normalize(glm::vec3(0, 1, -1)));		// The up direction of the camera.
glm::mat4 Window::view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);

glm::vec3 Window::prevPoint;

// Light source properties
glm::vec3 Window::lightPos(0, 20, 20);
glm::vec3 Window::lightColor(0.9, 0.9, 0.9);

// color list and color status
std::vector<glm::vec3> Window::colorList({
	glm::vec3((float)0x3e / 255, (float)0x47 / 255, (float)0x4e / 255),
	glm::vec3((float)0x13 / 255, (float)0x2e / 255, (float)0xd1 / 255),
	glm::vec3((float)0x71 / 255, (float)0x49 / 255, (float)0x1d / 255),
	glm::vec3((float)0x39 / 255, (float)0xfe / 255, (float)0xdd / 255),
	glm::vec3((float)0x13 / 255, (float)0x80 / 255, (float)0x2c / 255),
	glm::vec3((float)0x4e / 255, (float)0xef / 255, (float)0x38 / 255),
	glm::vec3((float)0xf1 / 255, (float)0x7d / 255, (float)0x0c / 255),
	glm::vec3((float)0xec / 255, (float)0x54 / 255, (float)0xbb / 255),
	glm::vec3((float)0x6c / 255, (float)0x2f / 255, (float)0xbc / 255),
	glm::vec3((float)0xc5 / 255, (float)0x12 / 255, (float)0x11 / 255),
	glm::vec3((float)0xd6 / 255, (float)0xdf / 255, (float)0xf1 / 255),
	glm::vec3((float)0xf6 / 255, (float)0xf6 / 255, (float)0x57 / 255),
});

std::vector<bool> Window::colorStatus(12, false);

// Shader Program ID
GLuint Window::phongShader; 
GLuint Window::toonShader; 
GLuint Window::particleShader;

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	phongShader = LoadShaders("shaders/phong.vert", "shaders/phong.frag");
	toonShader = LoadShaders("shaders/toon.vert", "shaders/toon.frag");
	particleShader = LoadShaders("shaders/particle.vert", "shaders/particle.frag");

	// Check the shader program.
	if (!phongShader || !toonShader || !particleShader)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// initialize random
	srand(time(NULL));

	// initialize scene graph of the ride
	world = new Transform(glm::mat4(1));
	auto world2Lobby = new Transform(glm::mat4(1));
	auto mainLobby = new Geometry("models/amongus_lobby.obj", phongShader, glm::vec3(0.2), glm::vec3(0.8, 0.8, 0.9), glm::vec3(0.2), glm::vec3(1));
	auto lobby2Astro = new Transform(glm::translate(glm::vec3(0, -4.3, 2)));
	auto astroFace = new Transform(glm::mat4(1));
	auto astro = new Geometry("models/amongus_astro_still.obj", toonShader, glm::vec3(0.1), colorList[5], glm::vec3(0), glm::vec3(1));
	colorStatus[5] = true;
	lobby2Astro->toggleMove();

	auto particle = new Particle(particleShader, glm::vec3(0, 1, 1), 150, 2);
	
	world->addChild(world2Lobby);
	world2Lobby->addChild(mainLobby);
	mainLobby->addChild(lobby2Astro);
	lobby2Astro->addChild(astroFace);
	lobby2Astro->addChild(particle);
	astroFace->addChild(astro);

	lobby = mainLobby;
	playerAstroMoveControl = lobby2Astro;
	playerAstroFaceControl = astroFace;
	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete world;

	// Delete the shader program.
	glDeleteProgram(phongShader);
	glDeleteProgram(toonShader);
	glDeleteProgram(particleShader);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(60.0), 
								double(width) / (double)height, 1.0, 1000.0);
}

// Perform any necessary updates here 
void Window::idleCallback()
{
	// move according to key pressed
	playerMovement();
	// move computer astros
	computerMovement();

	int toggleRandom = rand() % 100;
	if (toggleRandom < 10) {
		randomToggle();
	}

	int addRandom = rand() % 100;
	if (addRandom < 5) {
		randomAdd();
	}

	if (indexToRemove != -1) {
            randomRemove();
	}
	else {
            int removeRandom = rand() % 200;
            if (removeRandom < 1) {
                  randomRemove();
            }
	}

	// update all objects in scene graph
	world->update();
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the objects
	glUseProgram(phongShader);
	glUniformMatrix4fv(glGetUniformLocation(phongShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(phongShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3fv(glGetUniformLocation(phongShader, "eyePos"), 1, glm::value_ptr(eyePos));
	glUniform3fv(glGetUniformLocation(phongShader, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(phongShader, "lightColor"), 1, glm::value_ptr(lightColor));

	glUseProgram(toonShader);
	glUniformMatrix4fv(glGetUniformLocation(toonShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(toonShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3fv(glGetUniformLocation(toonShader, "eyePos"), 1, glm::value_ptr(eyePos));
	glUniform3fv(glGetUniformLocation(toonShader, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(toonShader, "lightColor"), 1, glm::value_ptr(lightColor));

	glUseProgram(particleShader);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// call draw on scene graph
	world->draw(glm::mat4(1));

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press.
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_W:
			keyPressed.wPressed = true;
			break;

		case GLFW_KEY_A:
			keyPressed.aPressed = true;
			break;

		case GLFW_KEY_S:
			keyPressed.sPressed = true;
			break;

		case GLFW_KEY_D:
			keyPressed.dPressed = true;
			break;

		default:
			break;
		}
	}

	else {
		keyPressed.wPressed = false;
		keyPressed.aPressed = false;
		keyPressed.sPressed = false;
		keyPressed.dPressed = false;
	}
}

// control key movement
void Window::playerMovement() {
	if (keyPressed.wPressed) {
		float angle = glm::radians(180.0);
		playerAstroMoveControl->move(angle);
		playerAstroFaceControl->face(angle);
		if (lobbyCollide(playerAstroMoveControl->getLocation(), angle) != 10.0 ||
			astroCollide(playerAstroMoveControl->getLocation(), angle) != 10.0) {
                  playerAstroMoveControl->move(glm::radians(0.0f));
		}
	} else if (keyPressed.aPressed) {
		float angle = glm::radians(270.0);
		playerAstroMoveControl->move(angle);
		playerAstroFaceControl->face(angle);
		if (lobbyCollide(playerAstroMoveControl->getLocation(), angle) != 10.0 ||
			astroCollide(playerAstroMoveControl->getLocation(), angle) != 10.0) {
                  playerAstroMoveControl->move(glm::radians(90.0f));
		}
	} else if (keyPressed.sPressed) {
		float angle = glm::radians(0.0);
		playerAstroMoveControl->move(angle);
		playerAstroFaceControl->face(angle);
		if (lobbyCollide(playerAstroMoveControl->getLocation(), angle) != 10.0 ||
			astroCollide(playerAstroMoveControl->getLocation(), angle) != 10.0) {
                  playerAstroMoveControl->move(glm::radians(180.0f));
		}
	} else if (keyPressed.dPressed) {
		float angle = glm::radians(90.0);
		playerAstroMoveControl->move(angle);
		playerAstroFaceControl->face(angle);
		if (lobbyCollide(playerAstroMoveControl->getLocation(), angle) != 10.0 ||
			astroCollide(playerAstroMoveControl->getLocation(), angle) != 10.0) {
                  playerAstroMoveControl->move(glm::radians(270.0f));
		}
	}

      view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);
}

void Window::computerMovement() {
	for (int i = 0; i < computerAstroMoveList.size(); ++i) {
		computerAstroMoveList[i]->move(angleList[i]);
		computerAstroFaceList[i]->face(angleList[i]);

		float astroReflectAngle = astroCollide(computerAstroMoveList[i]->getLocation(), angleList[i]);
		if (astroReflectAngle != 10.0) {
			angleList[i] = astroReflectAngle;
                  computerAstroMoveList[i]->move(angleList[i]);
                  computerAstroFaceList[i]->face(angleList[i]);
		}

		float lobbyReflectAngle = lobbyCollide(computerAstroMoveList[i]->getLocation(), angleList[i]);
		if (lobbyReflectAngle != 10.0) {
			angleList[i] = lobbyReflectAngle;
                  computerAstroMoveList[i]->move(angleList[i]);
                  computerAstroFaceList[i]->face(angleList[i]);
		}
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	// when left button pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// set flag to true
		keyPressed.mousePressed = true;
		// calculate pressed position in 3D
		double xpos;
		double ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		prevPoint = trackBallMapping(glm::vec2(xpos, ypos));
	}
	// else, set flag to false
	else {
		keyPressed.mousePressed = false;
	}
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	// when flag pressed is true
	if (keyPressed.mousePressed) {
		// get current screen position and calculate current position in 3D
		glm::vec2 currPos(xpos, ypos);
		glm::vec3 currPoint = trackBallMapping(currPos);

		// get velocity
		float vertVelocity = glm::length(currPoint - prevPoint);
		// only move if velocity exceeds some threshold
		if (vertVelocity > 0.001) {
			// get rot Axis
			glm::vec3 rotAxis = glm::cross(currPoint, prevPoint);
			rotAxis.y = 0;
			rotAxis.z = 0;
			if (rotAxis.x == 0) {
				return;
			}
			else {
				rotAxis.x = rotAxis.x > 0 ? 1 : -1;
			}

			glm::mat4 rotateMotion = glm::rotate(glm::mat4(1.0f), glm::radians(vertVelocity * 25.0f), rotAxis);
			upVector = glm::vec3(rotateMotion * glm::vec4(upVector, 0.0));
                  eyePos = glm::vec3(rotateMotion * glm::vec4(eyePos, 1.0));
                  view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);
			prevPoint = currPoint;
		}
	}
}

glm::vec3 Window::trackBallMapping(glm::vec2 point) {
	glm::vec3 v(0);
	double d;
	v.x = (2 * point.x - width) / width;
	v.y = (height - 2 * point.y) / height;
	v.z = 0;
	d = glm::length(v);
	d = (d < 1) ? d : 1;
	v.z = sqrt(1.001 - d * d);
	v = v / glm::length(v);
	return v;
}

float Window::lobbyCollide(glm::vec3 location, float angle) {
	// collide left side
	if (abs(location.x + 16) <= 1.1) {
		std::cerr << "Collide l side!" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::vec2(1, 0);
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// collide right side
	if (abs(location.x - 17) <= 1.1) {
		std::cerr << "Collide r side" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::vec2(-1, 0);
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// collide up side
	if (abs(location.z - 0) <= 1.1) {
		std::cerr << "Collide u side!" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::vec2(0, 1);
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// collide down side
	if (abs(location.z - 17) <= 1.1) {
		std::cerr << "Collide d side!" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::vec2(0, -1);
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// collide box 1
	if (glm::length(glm::vec2(location.x, location.z) - glm::vec2(-9, 7)) <= 1.1 + 2.5) {
		std::cerr << "Collide box 1!" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::normalize(glm::vec2(location.x, location.z) - glm::vec2(-9, 7));
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// collide box 2
	if (glm::length(glm::vec2(location.x, location.z) - glm::vec2(11, 4)) <= 1.1 + 2.5) {
		std::cerr << "Collide box 2!" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::normalize(glm::vec2(location.x, location.z) - glm::vec2(11, 4));
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// collide lower left diagonal line
	if (abs(4 * location.x + (-5) * location.z + 128) / glm::sqrt(4 * 4 + 5 * 5) <= 1.1) {
		std::cerr << "Collide diag 1!" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::normalize(glm::vec2(4, -5));
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// collide lower right diagonal line
	if (abs(4 * location.x + 5 * location.z - 130) / glm::sqrt(4 * 4 + 5 * 5) <= 1.1) {
		std::cerr << "Collide diag 2!" << std::endl;
		auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
		auto N = glm::normalize(glm::vec2(-4, -5));
		auto reflect = glm::reflect(I, N);
		return glm::atan(reflect.x, reflect.y);
	}

	// no collision flag
	return 10.0;
}

float Window::astroCollide(glm::vec3 location, float angle) {
	if (location.x != playerAstroMoveControl->getLocation().x ||
		location.y != playerAstroMoveControl->getLocation().y ||
		location.z != playerAstroMoveControl->getLocation().z) {
		auto centerVec = glm::vec2(location.x, location.z) -
			glm::vec2(playerAstroMoveControl->getLocation().x, playerAstroMoveControl->getLocation().z);
		if (glm::length(centerVec) <= 2) {
                  auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
			auto N = glm::normalize(centerVec);
                  auto reflect = glm::reflect(I, N);
                  return glm::atan(reflect.x, reflect.y);
		}
	}

	for (auto computerAstro : computerAstroMoveList) {
            if (location.x != computerAstro->getLocation().x ||
                  location.y != computerAstro->getLocation().y ||
                  location.z != computerAstro->getLocation().z) {

                  auto centerVec = glm::vec2(location.x, location.z) -
                        glm::vec2(computerAstro->getLocation().x, computerAstro->getLocation().z);
                  if (glm::length(centerVec) <= 2) {
                        auto I = glm::vec2(glm::sin(angle), glm::cos(angle));
                        auto N = glm::normalize(centerVec);
                        auto reflect = glm::reflect(I, N);
                        return glm::atan(reflect.x, reflect.y);
                  }
            }
	}

	return 10.0;
}

bool Window::initialAstroCollide(glm::vec3 location) {
		auto centerVec = glm::vec2(location.x, location.z) -
			glm::vec2(playerAstroMoveControl->getLocation().x, playerAstroMoveControl->getLocation().z);
		if (glm::length(centerVec) <= 2) {
			return true;
		}

	for (auto computerAstro : computerAstroMoveList) {
            auto centerVec = glm::vec2(location.x, location.z) -
                  glm::vec2(computerAstro->getLocation().x, computerAstro->getLocation().z);
            if (glm::length(centerVec) <= 2) {
			return true;
            }
	}

	return false;
}

void Window::randomAdd() {
	if (computerAstroMoveList.size() == 10) {
		return;
	}

	float randomX = (float) rand() / RAND_MAX * 30 - 15;
	float fixY = -4.3;
	float randomZ = (float) rand() / RAND_MAX * 10;
	auto randomLoc = glm::vec3(randomX, fixY, randomZ);
	std::cerr << randomX << ", " << randomZ << std::endl;

	while (lobbyCollide(randomLoc, 0) != 10.0 || initialAstroCollide(randomLoc)) {
            randomX = (float) rand() / RAND_MAX * 30 - 15;
		randomZ = (float) rand() / RAND_MAX * 10;
		randomLoc = glm::vec3(randomX, fixY, randomZ);
	}

	// particle effect appear
      auto lobby2ComputerAstro = new Transform(glm::translate(randomLoc));
      auto computerAstroFace = new Transform(glm::mat4(1));

	int randomColorIndex = rand() % 12;
	while (colorStatus[randomColorIndex]) {
		randomColorIndex = rand() % 12;
	}
      auto computerAstro = new Geometry("models/amongus_astro_still.obj", toonShader, glm::vec3(0.1), colorList[randomColorIndex], glm::vec3(0), glm::vec3(1));
	colorStatus[randomColorIndex] = true;

	auto particle = new Particle(particleShader, glm::vec3(0, 1, 1), 150, 2);

      lobby->addChild(lobby2ComputerAstro);
      lobby2ComputerAstro->addChild(computerAstroFace);
	lobby2ComputerAstro->addChild(particle);
      computerAstroFace->addChild(computerAstro);

      computerAstroMoveList.push_back(lobby2ComputerAstro);
      computerAstroFaceList.push_back(computerAstroFace);
	particleList.push_back(particle);

	float randomAngle = glm::radians((float) rand() / RAND_MAX * 360.0);
	angleList.push_back(randomAngle);
	colorIndexList.push_back(randomColorIndex);
}

void Window::randomRemove() {
	if (computerAstroMoveList.size() == 0) {
		return;
	}

	// particle effect disappear
	if (removeDelay == 200) {
            indexToRemove = rand() % computerAstroMoveList.size();
		particleList[indexToRemove]->resetCounter();
	}
	if (removeDelay > 0) {
		--removeDelay;
	}
	else {
            lobby->removeChild(computerAstroMoveList[indexToRemove]);
            computerAstroMoveList.erase(computerAstroMoveList.begin() + indexToRemove);
            computerAstroFaceList.erase(computerAstroFaceList.begin() + indexToRemove);
            particleList.erase(particleList.begin() + indexToRemove);
            angleList.erase(angleList.begin() + indexToRemove);

            colorStatus[colorIndexList[indexToRemove]] = false;
            colorIndexList.erase(colorIndexList.begin() + indexToRemove);
		removeDelay = 200;
		indexToRemove = -1;
	}
}

void Window::randomToggle() {
	if (computerAstroMoveList.size() == 0) {
		return;
	}

      int index = rand() % computerAstroMoveList.size();
	computerAstroMoveList[index]->toggleMove();
}
