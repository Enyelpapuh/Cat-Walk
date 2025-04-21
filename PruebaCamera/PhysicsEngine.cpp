#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(glm::vec3 grav) : gravity(grav) {}

void PhysicsEngine::addObject(RigidBody* object) {
    objects.push_back(object);
}

void PhysicsEngine::update(float dt) {
    for (auto& obj : objects) {
        obj->update(dt, gravity);
    }

    // Detección y manejo de colisiones
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            if (objects[i]->checkCollision(*objects[j])) {
                handleCollision(objects[i], objects[j]);
            }
        }
    }
}

void PhysicsEngine::handleCollision(RigidBody* obj1, RigidBody* obj2) {
    glm::vec3 normal = glm::normalize(obj1->position - obj2->position);
    glm::vec3 relativeVelocity = obj1->velocity - obj2->velocity;

    float velocityAlongNormal = glm::dot(relativeVelocity, normal);

    if (velocityAlongNormal > 0) return;

    float e = 0.7f;
    float j = -(1 + e) * velocityAlongNormal;
    j /= (1 / obj1->mass + 1 / obj2->mass);

    glm::vec3 impulse = j * normal;

    obj1->velocity += impulse / obj1->mass;
    obj2->velocity -= impulse / obj2->mass;
}
