#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <vector>
#include "DebugDrawer.h"

// Forward declarations
class Model;

class PhysicsManager {
public:
    PhysicsManager();
    ~PhysicsManager();

    void StepSimulation(float deltaTime);
    void DebugDrawWorld();                  // <── NUEVO
    DebugDrawer* GetDebugDrawer() const { return debugDrawer; }

    // Crea un cubo dinámico que sigue al modelo animado
    btRigidBody* CreateDynamicBox(const glm::vec3& position, const glm::vec3& halfExtents, float mass);

    // Crea un colisionador estático desde un modelo 3D (ej. el mapa)
    btRigidBody* CreateStaticMeshFromModel(const Model& model,
        const glm::vec3& scale,
        const glm::vec3& eulerRotation);


    btDiscreteDynamicsWorld* GetDynamicsWorld() const { return dynamicsWorld; }


private:
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    std::vector<btCollisionShape*> collisionShapes;
    DebugDrawer* debugDrawer = nullptr;     // linea para draw hitbox
};

#endif
