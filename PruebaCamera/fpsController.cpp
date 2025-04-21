

#include "fpsController.h"

FPSController::FPSController()
{
    m_transform = Transform3D();
}

FPSController::~FPSController()
{
}

Transform3D FPSController::GetTransform()
{
    return m_transform;
}

// fpsController.cpp
void FPSController::Update(GLFWwindow* window, glm::vec2 viewportDimensions, glm::vec2 mousePosition, float deltaTime, Transform3D& transform)
{
    glm::vec2 mouseMovement = mousePosition - (viewportDimensions / 2.0f);

    float yaw = transform.Rotation().y;
    yaw += (int)mouseMovement.x * .001f;

    float pitch = transform.Rotation().x;
    pitch -= (int)mouseMovement.y * .001f;

    float halfpi = 3.1416f / 2.f;
    if (pitch < -halfpi) pitch = -halfpi;
    else if (pitch > halfpi) pitch = halfpi;

    transform.SetRotation(glm::vec3(pitch, yaw, 0));

    glfwSetCursorPos(window, mousePosition.x - mouseMovement.x, mousePosition.y - mouseMovement.y);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        transform.Translate(transform.GetForward() * 5.0f * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        transform.Translate(transform.GetRight() * -5.0f * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        transform.Translate(transform.GetForward() * -5.0f * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        transform.Translate(transform.GetRight() * 5.0f * deltaTime);
    }
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		transform.Translate(glm::vec3(0, 5.0f * deltaTime, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		transform.Translate(glm::vec3(0, -5.0f * deltaTime, 0));
	}
}

