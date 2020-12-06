#include "Transform.h"

Transform::Transform(const glm::mat4& transMatrix) :
      transform(transMatrix) {}

Transform::~Transform() {
      for (auto child : children) {
            delete child;
      }
}

void Transform::draw(const glm::mat4& C, GLuint shader) {
      for (auto child : children) {
            child->draw(C * transform, shader);
      }
}

void Transform::update() {
      for (auto child : children) {
            child->update();
      }
}

void Transform::addChild(Node* child) {
      children.push_back(child);
}