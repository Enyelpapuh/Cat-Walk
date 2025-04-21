#include "RigidBody.h"
#include <iostream>

RigidBody::RigidBody(glm::vec3 pos, glm::vec3 vel, float m, float s)
    : position(pos), velocity(vel), mass(m), size(s) {
}

void RigidBody::update(float dt, const glm::vec3& gravity) {
    velocity += gravity * dt;
    position += velocity * dt;

    float halfSize = size * 0.5f;
    if (position.y - halfSize <= 0.0f) {
        position.y = halfSize;
        velocity.y *= -0.7f;
    }
}

bool RigidBody::checkCollision(const RigidBody& other) {
    float distance = glm::length(position - other.position);
    float combinedSize = (size + other.size) * 0.5f;
    return distance <= combinedSize;
}
