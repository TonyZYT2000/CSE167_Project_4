#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "Node.h"
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>

class Particle : public Node
{
private:
	GLuint shader;
	glm::vec3 color;
	float pointSize;
	glm::mat4 model;
	std::vector<glm::vec3> points;
	GLuint VAO, VBO;

	int counter;

public:
	Particle(GLuint shader, glm::vec3 color, int count, float pointSize);
	~Particle();
	void draw(const glm::mat4& C);
	void update();
	void spin(float deg);
	void resetCounter();
};

#endif
