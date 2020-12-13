#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "GLFW Starter Project";

// Objects to Render
//Sphere* Window::disco;
Transform* Window::world;
Transform* Window::astroMoveControl;
Transform* Window::astroFaceControl;

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

float Window::speed = 1;
glm::vec3 Window::prevPoint;

// Light source properties
glm::vec3 Window::lightPos(0, 20, 20);
glm::vec3 Window::lightColor(0.9, 0.9, 0.9);

// Shader Program ID
GLuint Window::phongShader; 
GLuint Window::toonShader; 

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	phongShader = LoadShaders("shaders/phong.vert", "shaders/phong.frag");
	toonShader = LoadShaders("shaders/toon.vert", "shaders/toon.frag");

	// Check the shader program.
	if (!phongShader || !toonShader)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// initialize scene graph of the ride
	world = new Transform(glm::mat4(1));
	auto world2Lobby = new Transform(glm::mat4(1));
	auto lobby = new Geometry("models/amongus_lobby.obj", phongShader, glm::vec3(0.2), glm::vec3(0.8, 0.8, 0.9), glm::vec3(0.2), glm::vec3(1));
	auto lobby2Astro = new Transform(glm::translate(glm::vec3(0, -4.3, 2)));
	auto astroFace = new Transform(glm::mat4(1));
	auto astro = new Geometry("models/amongus_astro_still.obj", toonShader, glm::vec3(0.1), glm::vec3(0.31, 1, 0.22), glm::vec3(0), glm::vec3(1));
	
	world->addChild(world2Lobby);
	world2Lobby->addChild(lobby);
	lobby->addChild(lobby2Astro);
	lobby2Astro->addChild(astroFace);
	astroFace->addChild(astro);

	astroMoveControl = lobby2Astro;
	astroFaceControl = astroFace;
	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete world;

	// Delete the shader program.
	glDeleteProgram(phongShader);
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
	// Move according to key pressed
	movement();
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

		case GLFW_KEY_P:
			keyPressed.pPressed = true;
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
		keyPressed.pPressed = false;
	}
}

// control key movement
void Window::movement() {
	if (keyPressed.wPressed) {
		astroMoveControl->move(glm::radians(180.0f));
		astroFaceControl->face(glm::radians(180.0f));
		if (lobbyCollide(astroMoveControl->getLocation())) {
                  astroMoveControl->move(glm::radians(0.0f));
		}
	} else if (keyPressed.aPressed) {
		astroMoveControl->move(glm::radians(270.0f));
		astroFaceControl->face(glm::radians(270.0f));
		if (lobbyCollide(astroMoveControl->getLocation())) {
                  astroMoveControl->move(glm::radians(90.0f));
		}
	} else if (keyPressed.sPressed) {
		astroMoveControl->move(glm::radians(0.0f));
		astroFaceControl->face(glm::radians(0.0f));
		if (lobbyCollide(astroMoveControl->getLocation())) {
                  astroMoveControl->move(glm::radians(180.0f));
		}
	} else if (keyPressed.dPressed) {
		astroMoveControl->move(glm::radians(90.0f));
		astroFaceControl->face(glm::radians(90.0f));
		if (lobbyCollide(astroMoveControl->getLocation())) {
                  astroMoveControl->move(glm::radians(270.0f));
		}
	}

	if (keyPressed.pPressed) {
		glm::vec3 location = astroMoveControl->getLocation();
		std::cerr << location.x << ", " << location.y << ", " << location.z << std::endl;
		std::cerr << upVector.x << ", " << upVector.y << ", " << upVector.z << std::endl;
		std::cerr << eyePos.x << ", " << eyePos.y << ", " << eyePos.z << std::endl;
	}

      view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);
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

bool Window::lobbyCollide(glm::vec3 location) {
	if (abs(location.x + 16) <= 1.1 || abs(location.x - 17) <= 1.1) {
		std::cerr << "Collide lr side!" << std::endl;
		return true;
	}

	if (abs(location.z - 0) <= 1.1 || abs(location.z - 17) <= 1.1) {
		std::cerr << "Collide ud side!" << std::endl;
		return true;
	}

	if (glm::length(glm::vec2(location.x, location.z) - glm::vec2(-9, 7)) <= 1.1 + 2.5) {
		std::cerr << "Collide box 1!" << std::endl;
		return true;
	}

	if (glm::length(glm::vec2(location.x, location.z) - glm::vec2(11, 4)) <= 1.1 + 2.5) {
		std::cerr << "Collide box 2!" << std::endl;
		return true;
	}

	if (abs(4 * location.x + (-5) * location.z + 128) / glm::sqrt(4 * 4 + 5 * 5) <= 1.1) {
		std::cerr << "Collide diag 1!" << std::endl;
		return true;
	}

	if (abs(4 * location.x + 5 * location.z - 130) / glm::sqrt(4 * 4 + 5 * 5) <= 1.1) {
		std::cerr << "Collide diag 2!" << std::endl;
		return true;
	}

	return false;
}
