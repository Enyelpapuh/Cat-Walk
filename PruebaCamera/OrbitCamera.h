#pragma once
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class OrbitCamera {
private:
    float radius;
    float angleX, angleY;
    glm::vec3 target;
    glm::vec2 lastMousePos;
    bool firstMouse;

public:
    OrbitCamera(float r = 10.0f, glm::vec3 t = glm::vec3(0.0f));

    glm::mat4 GetViewMatrix();
    void Update(GLFWwindow* window, float dt);
    void SetTarget(glm::vec3 t);
    void Zoom(float offset);
};
