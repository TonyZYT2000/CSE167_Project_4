#include "Particle.h"

Particle::Particle(GLuint shader, glm::vec3 color, int count, float pointSize) :
	shader(shader), color(color), pointSize(pointSize), counter(0) {
      model = glm::mat4(1);
	srand(time(0));

      for (int i = 0; i < count; ++i) {
		auto x = (float)rand() / RAND_MAX * 2 - 1;
		auto y = (float)rand() / RAND_MAX;
		auto z = (float)rand() / RAND_MAX * 2 - 1;
		points.push_back(glm::vec3(x, y, z));
      }

	// Generate a Vertex Array (VAO) and Vertex Buffer Object (VBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the point data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass point data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Particle::~Particle() 
{
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Particle::draw(const glm::mat4& C)
{
	if (counter < 200 || counter > 250) {
            // Actiavte the shader program 
            glUseProgram(shader);

            // Get the shader variable locations and send the uniform data to the shader 
            glUniformMatrix4fv(glGetUniformLocation(shader, "transform"), 1, GL_FALSE, glm::value_ptr(C));
            glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

            // Bind the VAO
            glBindVertexArray(VAO);

            // Set point size
            glPointSize(pointSize);

            // Draw the points 
            glDrawArrays(GL_POINTS, 0, points.size());

            // Unbind the VAO and shader program
            glBindVertexArray(0);
            glUseProgram(0);
	}
	else {
		return;
	}
}

void Particle::update()
{
	if (counter < 200) {
            // Spin the cube by 1 degree
            spin(glm::radians(5.0f));
            model = glm::scale(glm::vec3(1.005)) * model;
            pointSize += 0.01;
            ++counter;
	}
	else if (counter > 250 && counter < 500) {
            spin(glm::radians(-5.0f));
            model = glm::scale(glm::vec3(0.995)) * model;
            pointSize += 0.01;
            ++counter;
	}
}

void Particle::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = model * glm::rotate(deg, glm::vec3(0, 1, 0));
}

void Particle::resetCounter() {
      color = glm::vec3(1, 0, 0);
	counter = 300;
}