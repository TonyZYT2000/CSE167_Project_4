#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Node.h"
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Geometry : public Node
{
private:
	int mode;
	int modeMem = 0;
	glm::mat4 model;
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> faces;
	glm::vec3 kAmbient;
	glm::vec3 kDiffuse;
	glm::vec3 kSpecular;

	GLuint VAO, VBO, NBO, EBO;

	std::list<Node*> children;

public:
	Geometry(std::string objFilename, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 scale, int updateMode);
	~Geometry();
	void draw(const glm::mat4& C, GLuint shader);
	void update();
	void addChild(Node* child);
	void toggleMove();
};

#endif
