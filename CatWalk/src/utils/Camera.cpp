#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(glm::vec3 startPosition, glm::vec3 upDirection, float startYaw, float startPitch,
    float startSpeed, float startSensitivity, float startZoom)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(upDirection), yaw(startYaw), pitch(startPitch),
    movementSpeed(startSpeed), mouseSensitivity(startSensitivity), zoom(startZoom), position(startPosition)
{
    // Llamar a la función para actualizar los vectores de la cámara
    updateCameraVectors();
}

// Procesamiento de entrada del teclado
void Camera::processKeyboardInput(GLFWwindow* window, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    // Movimiento hacia adelante y hacia atrás
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;

    // Movimiento lateral (izquierda/derecha)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;

	// Movimiento hacia arriba y hacia abajo
    if(glfwGetKey(window, GLFW_KEY_SPACE)== GLFW_PRESS)
		position += up * velocity;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= up * velocity;
	// Movimiento rápido
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		position += front * velocity * 2.0f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// Procesamiento del movimiento del ratón
void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    // Aplicar sensibilidad del ratón
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    // Ajustar las orientaciones de la cámara según el desplazamiento del ratón
    yaw += xoffset;
    pitch += yoffset;

    // Restringir el ángulo del pitch para evitar volcarse
    if (constrainPitch) {
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    // Actualizar los vectores de la cámara
    updateCameraVectors();
}

// Procesamiento del zoom (rueda del ratón)
void Camera::processMouseScroll(float yoffset)
{
    zoom -= (float)yoffset;

    // Restringir el zoom dentro de un rango
    zoom = glm::clamp(zoom, 1.0f, 45.0f);
}

// Obtener la matriz de vista de la cámara
glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

// Actualizar los vectores de dirección de la cámara
void Camera::updateCameraVectors()
{
    // Calcular el nuevo vector front basado en el yaw y pitch
    glm::vec3 frontTemp;
    frontTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontTemp.y = sin(glm::radians(pitch));
    frontTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(frontTemp);

    // Calcular los vectores right y up
    right = glm::normalize(glm::cross(front, worldUp)); // Right vector es perpendicular a front y up
    up = glm::normalize(glm::cross(right, front));      // Up vector es perpendicular a right y front
}
