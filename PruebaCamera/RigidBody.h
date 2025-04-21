#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>

struct RigidBody {
    glm::vec3 position;
    glm::vec3 velocity;
    float mass;
    float size;

    RigidBody(glm::vec3 pos, glm::vec3 vel, float m, float s = 1.0f);

    void update(float dt, const glm::vec3& gravity);
    bool checkCollision(const RigidBody& other);
};

#endif // RIGIDBODY_H
