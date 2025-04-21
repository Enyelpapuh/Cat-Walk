#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "RigidBody.h"
#include <vector>

class PhysicsEngine {
public:
    std::vector<RigidBody*> objects;
    glm::vec3 gravity;

    PhysicsEngine(glm::vec3 grav);

    void addObject(RigidBody* object);
    void update(float dt);
    void handleCollision(RigidBody* obj1, RigidBody* obj2);
};

#endif // PHYSICSENGINE_H
