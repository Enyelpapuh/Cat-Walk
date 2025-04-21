#pragma once

#include "Transform3D.h"
#include <GLFW/glfw3.h>

class CharacterController {
public:
    // Constructor
    CharacterController();

    // Destructor
    ~CharacterController();

    // Devuelve el transform del personaje (su posici�n, rotaci�n, escala)
    Transform3D GetTransform();

    // Actualiza el controlador con entrada de teclado y rat�n
    void Update(GLFWwindow* window, glm::vec2 viewportDimensions, glm::vec2 mousePosition, float deltaTime);

    // Establece la velocidad del personaje
    void SetSpeed(float speed);

    // Establece la sensibilidad del rat�n
    void SetMouseSensitivity(float sensitivity);

private:
    Transform3D m_transform;  // Transform del personaje (posicion, rotaci�n, escala)
    float m_speed;            // Velocidad del movimiento
    float m_sensitivity;      // Sensibilidad del rat�n

    // Para control de rat�n
    glm::vec2 m_lastMousePos;
    bool m_firstMouse;
};

