
#pragma once
#include "transform3d.h"
#include "GLFW/glfw3.h"


class FPSController {

private:
    Transform3D m_transform;

public:
    FPSController();
    ~FPSController();
    Transform3D GetTransform();
    void Update(GLFWwindow* window, glm::vec2 viewportDimensions, glm::vec2 mousePosition, float deltaTime, Transform3D& targetTransform);








};