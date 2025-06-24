#include "PhysicsManager.h"
#include "model_animation.h"
#include "Mesh.h"

PhysicsManager::PhysicsManager() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));

    // ---- Debug drawer ---------------------------------
    debugDrawer = new DebugDrawer();
    dynamicsWorld->setDebugDrawer(debugDrawer);
}

PhysicsManager::~PhysicsManager() {
    for (btCollisionShape* shape : collisionShapes)
        delete shape;

    delete debugDrawer;
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void PhysicsManager::StepSimulation(float deltaTime) {
    /*const float fixedTimeStep = 1.0f / 60.0f;
    int maxSubSteps = 5;*/
    dynamicsWorld->stepSimulation(deltaTime, 8);
}

btRigidBody* PhysicsManager::CreateDynamicBox(const glm::vec3& position, const glm::vec3& halfExtents, float mass) {
    btCollisionShape* boxShape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
    collisionShapes.push_back(boxShape);

    btVector3 localInertia(0, 0, 0);
    boxShape->calculateLocalInertia(mass, localInertia);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(position.x, position.y, position.z));

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, boxShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setActivationState(DISABLE_DEACTIVATION);//para evitar que el objeto se duerma

    dynamicsWorld->addRigidBody(body);
    return body;
}

btRigidBody* PhysicsManager::CreateStaticMeshFromModel(
    const Model& model,
    const glm::vec3& scale,
    const glm::vec3& eulerDeg)
{
    /* -------- matrices de usuario -------- */
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

    glm::mat4 R = glm::rotate(glm::mat4(1.0f),
        glm::radians(eulerDeg.x), glm::vec3(1, 0, 0)) *
        glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerDeg.y), glm::vec3(0, 1, 0)) *
        glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerDeg.z), glm::vec3(0, 0, 1));

    /* -------- triángulos -------- */
    btTriangleMesh* tri = new btTriangleMesh();

    for (const auto& m : model.meshes)
    {
        for (size_t i = 0; i < m.indices.size(); i += 3)
        {
            auto V = [&](size_t idx)
                {
                    glm::vec4 v(m.vertices[idx].Position, 1.0f);
                    v = R * S * v;                      // ESCALA + ROTACIÓN
                    return btVector3(v.x, v.y, v.z);
                };

            tri->addTriangle(V(m.indices[i]),
                V(m.indices[i + 1]),
                V(m.indices[i + 2]));
        }
    }

    /* -------- shape y rigid body -------- */
    auto* shape = new btBvhTriangleMeshShape(tri, true);
    collisionShapes.push_back(shape);

    btRigidBody* body = new btRigidBody(
        btRigidBody::btRigidBodyConstructionInfo(
            0.0f,                                 // masa 0 = estático
            new btDefaultMotionState(btTransform::getIdentity()),
            shape,
            btVector3(0, 0, 0)));

    body->setFriction(1.0f);
    body->setRestitution(0.0f);
    dynamicsWorld->addRigidBody(body);
    return body;
}


void PhysicsManager::DebugDrawWorld()
{
    debugDrawer->Clear();          // vacía buffer de líneas
    dynamicsWorld->debugDrawWorld();
}