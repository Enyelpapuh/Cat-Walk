#pragma once

#include "Transform3D.h"
#include <GLFW/glfw3.h>

class CharacterController {
public:
    // Constructor
    CharacterController();

    // Destructor
    ~CharacterController();

    // Devuelve el transform del personaje (su posición, rotación, escala)
    Transform3D GetTransform();

    // Actualiza el controlador con entrada de teclado y ratón
    void Update(GLFWwindow* window, glm::vec2 viewportDimensions, glm::vec2 mousePosition, float deltaTime);

    // Establece la velocidad del personaje
    void SetSpeed(float speed);

    // Establece la sensibilidad del ratón
    void SetMouseSensitivity(float sensitivity);

private:
    Transform3D m_transform;  // Transform del personaje (posicion, rotación, escala)
    float m_speed;            // Velocidad del movimiento
    float m_sensitivity;      // Sensibilidad del ratón

    // Para control de ratón
    glm::vec2 m_lastMousePos;
    bool m_firstMouse;
};

