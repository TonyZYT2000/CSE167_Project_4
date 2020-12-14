#include "Transform.h"

Transform::Transform(const glm::mat4& transMatrix) :
      transform(transMatrix), dirRecord(transform) {}

Transform::~Transform() {
      for (auto child : children) {
            delete child;
      }
}

void Transform::draw(const glm::mat4& C) {
      for (auto child : children) {
            child->draw(C * transform);
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

void Transform::move(float angle) {
      transform = glm::translate(glm::vec3(0.1 * glm::sin(angle), 0, 0.1 * glm::cos(angle))) * transform;
}

void Transform::face(float angle) {
      transform = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * dirRecord;
}

glm::vec3 Transform::getLocation() {
      return glm::vec3(transform * glm::vec4(0, 0, 0, 1));
}

int Transform::getCount() {
      return children.size();
}

void Transform::removeChild(Node* child) {
      delete child;
      children.remove(child);
}