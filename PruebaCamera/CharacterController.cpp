#include "CharacterController.h"
#include <GLFW/glfw3.h>

CharacterController::CharacterController()
    : m_speed(5.0f), m_sensitivity(0.1f), m_firstMouse(true) {
}

CharacterController::~CharacterController() {}

Transform3D CharacterController::GetTransform() {
    return m_transform;
}

void CharacterController::Update(GLFWwindow* window, glm::vec2 viewportDimensions, glm::vec2 mousePosition, float deltaTime) {
    // Movimiento con el teclado
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_transform.Translate(m_transform.GetForward() * m_speed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_transform.Translate(m_transform.GetForward() * -m_speed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_transform.Translate(m_transform.GetRight() * -m_speed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_transform.Translate(m_transform.GetRight() * m_speed * deltaTime);
    }

    // Rotación de la cámara con el ratón
    glm::vec2 mouseMovement = mousePosition - (viewportDimensions / 2.0f);

    if (m_firstMouse) {
        m_lastMousePos = mousePosition;
        m_firstMouse = false;
    }

    glm::vec2 offset = mousePosition - m_lastMousePos;
    m_lastMousePos = mousePosition;

    // Ajustar la rotación
    float yaw = m_transform.Rotation().y + offset.x * m_sensitivity;
    float pitch = m_transform.Rotation().x - offset.y * m_sensitivity;

    // Limitar el ángulo vertical (para evitar que la cámara se voltee completamente)
    float halfpi = 3.1416f / 2.f;
    if (pitch < -halfpi) pitch = -halfpi;
    if (pitch > halfpi) pitch = halfpi;

    m_transform.SetRotation(glm::vec3(pitch, yaw, 0));

    // Centrar el cursor en la pantalla
    glfwSetCursorPos(window, viewportDimensions.x / 2.0f, viewportDimensions.y / 2.0f);
}

void CharacterController::SetSpeed(float speed) {
    m_speed = speed;
}

void CharacterController::SetMouseSensitivity(float sensitivity) {
    m_sensitivity = sensitivity;
}
