#include "OrbitCamera.h"

OrbitCamera::OrbitCamera(float r, glm::vec3 t)
    : radius(r), angleX(0), angleY(0), target(t), firstMouse(true) {
}

glm::mat4 OrbitCamera::GetViewMatrix() {
    float x = radius * cos(glm::radians(angleY)) * sin(glm::radians(angleX));
    float y = radius * sin(glm::radians(angleY));
    float z = radius * cos(glm::radians(angleY)) * cos(glm::radians(angleX));

    glm::vec3 cameraPos = target + glm::vec3(x, y, z);
    return glm::lookAt(cameraPos, target, glm::vec3(0, 1, 0));
}

void OrbitCamera::Update(GLFWwindow* window, float dt) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastMousePos = glm::vec2(xpos, ypos);
        firstMouse = false;
    }

    float sensitivity = 0.2f;
    glm::vec2 offset = glm::vec2(xpos, ypos) - lastMousePos;
    lastMousePos = glm::vec2(xpos, ypos);

    angleX += offset.x * sensitivity;
    angleY -= offset.y * sensitivity;

    // Limita el ángulo vertical
    if (angleY > 89.0f) angleY = 89.0f;
    if (angleY < -89.0f) angleY = -89.0f;
}

void OrbitCamera::SetTarget(glm::vec3 t) {
    target = t;
}

void OrbitCamera::Zoom(float offset) {
    radius -= offset;
    if (radius < 1.0f) radius = 1.0f;
    if (radius > 50.0f) radius = 50.0f;
}
