#pragma once
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <glm/glm.hpp>

class DebugDrawer : public btIDebugDraw {
public:
    std::vector<glm::vec3> lines;
    int debugMode;

    DebugDrawer() : debugMode(DBG_DrawWireframe) {}

    // Bullet te pasa el color, pero dibujaremos todo rojo, así que lo ignoramos
    void drawLine(const btVector3& from, const btVector3& to,
        const btVector3& /*color*/) override
    {
        lines.emplace_back(from.x(), from.y(), from.z());
        lines.emplace_back(to.x(), to.y(), to.z());
    }

    // Los demás métodos los dejamos vacíos
    void drawContactPoint(const btVector3&, const btVector3&,
        btScalar, int, const btVector3&) override {
    }
    void reportErrorWarning(const char*) override {}
    void draw3dText(const btVector3&, const char*) override {}

    void setDebugMode(int mode) override { debugMode = mode; }
    int  getDebugMode() const override { return debugMode; }

    void Clear() { lines.clear(); }
};
