#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "GLFW Starter Project";

// Objects to Render
//Sphere* Window::disco;
Transform* Window::world;

// Track key pressed
KeyRecord Window::keyPressed;

// Camera Matrices 
// Projection matrix:
glm::mat4 Window::projection; 

// View Matrix:
glm::vec3 Window::eyePos(0, 20, 20);			// Camera position.
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

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	phongShader = LoadShaders("shaders/phong.vert", "shaders/phong.frag");

	// Check the shader program.
	if (!phongShader)
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
	auto world2Station = new Transform(glm::mat4(1));
	auto station = new Geometry("models/amongus_lobby.obj", glm::vec3(0.1), glm::vec3(0.9), glm::vec3(0.2), glm::vec3(1));
	
	world->addChild(world2Station);
	world2Station->addChild(station);

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
	// call draw on scene graph
	world->draw(glm::mat4(1), phongShader);

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

		case GLFW_KEY_Q:
			keyPressed.qPressed = true;
			break;

		case GLFW_KEY_E:
			keyPressed.ePressed = true;
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

		case GLFW_KEY_LEFT_CONTROL:
			keyPressed.ctrlPressed = true;
			break;

		case GLFW_KEY_LEFT_SHIFT:
			keyPressed.shiftPressed = true;
			break;

		default:
			break;
		}
	}

	else {
		keyPressed.qPressed = false;
		keyPressed.ePressed = false;
		keyPressed.wPressed = false;
		keyPressed.aPressed = false;
		keyPressed.sPressed = false;
		keyPressed.dPressed = false;
		keyPressed.ctrlPressed = false;
		keyPressed.shiftPressed = false;
	}
}

// control key movement
void Window::movement() {
	if (keyPressed.qPressed) {
		glm::mat4 rotate = glm::rotate(glm::mat4(1), glm::radians(0.5f), upVector);
		lookAtPoint = eyePos + glm::vec3(rotate * glm::vec4(lookAtPoint - eyePos, 0));
	} else if (keyPressed.ePressed) {
		glm::mat4 rotate = glm::rotate(glm::mat4(1), glm::radians(-0.5f), upVector);
		lookAtPoint = eyePos + glm::vec3(rotate * glm::vec4(lookAtPoint - eyePos, 0));
	}

	if (keyPressed.wPressed) {
		glm::vec3 forward = glm::normalize(lookAtPoint - eyePos);
		lookAtPoint += forward;
		eyePos += forward;
	}

	if (keyPressed.aPressed) {
		glm::vec3 leftward = glm::normalize(glm::cross(upVector, lookAtPoint - eyePos));
		lookAtPoint += leftward;
		eyePos += leftward;
	}

	if (keyPressed.sPressed) {
		glm::vec3 backward = glm::normalize(eyePos - lookAtPoint);
		lookAtPoint += backward;
		eyePos += backward;
	}

	if (keyPressed.dPressed) {
		glm::vec3 rightward = glm::normalize(glm::cross(lookAtPoint - eyePos, upVector));
		lookAtPoint += rightward;
		eyePos += rightward;
	}

	if (keyPressed.ctrlPressed) {
		glm::vec3 downward = -upVector;
		lookAtPoint += downward;
		eyePos += downward;
	}

	if (keyPressed.shiftPressed) {
		glm::vec3 upward = upVector;
		lookAtPoint += upward;
		eyePos += upward;
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
		if (vertVelocity > 0.0001) {
			// get rot Axis
			glm::vec3 rotAxis = glm::cross(currPoint, prevPoint);
			rotAxis.y = 0;
			rotAxis.z = 0;
			rotAxis = glm::normalize(rotAxis);

			glm::mat4 rotateMotion = glm::rotate(glm::mat4(1), glm::radians(vertVelocity * 25.0f), rotAxis);
			upVector = glm::vec3(rotateMotion * glm::vec4(upVector, 0.0));
                  eyePos = glm::vec3(rotateMotion * glm::vec4(eyePos, 0));
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

