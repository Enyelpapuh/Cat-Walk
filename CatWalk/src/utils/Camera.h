#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    // Atributos de la c�mara
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Par�metros de movimiento de c�mara
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // Constructor
    Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f),
        float startYaw = -90.0f,
        float startPitch = 0.0f,
        float startSpeed = 2.5f,
        float startSensitivity = 0.1f,
        float startZoom = 45.0f);

    // M�todos para actualizar la c�mara
    void processKeyboardInput(GLFWwindow* window, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);

    // Obtener la matriz de vista de la c�mara
    glm::mat4 getViewMatrix();

    // Obtener la posici�n de la c�mara
    glm::vec3 getPosition() { return position; }

private:
    void updateCameraVectors();
};

#endif