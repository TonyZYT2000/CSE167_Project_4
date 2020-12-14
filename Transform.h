#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Node.h"
#include <iostream>
#include <list>

class Transform : public Node
{
private:
	glm::mat4 transform;
	glm::mat4 dirRecord;
	std::list<Node*> children;

public:
	Transform(const glm::mat4& transMatrix);
	~Transform();
	void draw(const glm::mat4& C);
	void update();
	void addChild(Node* child);
	void move(float angle);
	void face(float angle);
	glm::vec3 getLocation();

	int getCount();
	void removeChild(Node* child);
};

#endif
