#ifndef CAMERA_AND_MODEL_CONTROLLER_H
#define CAMERA_AND_MODEL_CONTROLLER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Define camera movement directions
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    glm::vec3 Target = glm::vec3(0.0f, 0.0f, 0.0f);
    float Distance = 5.0f;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraPosition();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraPosition();
    }

    glm::mat4 GetViewMatrix()
    {
        updateCameraPosition();
        return glm::lookAt(Position, Target, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }

        updateCameraPosition();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Distance -= yoffset;
        if (Distance < 2.0f) Distance = 2.0f;
        if (Distance > 50.0f) Distance = 50.0f;

        updateCameraPosition();
    }

private:
    void updateCameraPosition()
    {
        float yawRad = glm::radians(Yaw);
        float pitchRad = glm::radians(Pitch);

        Position.x = Target.x + Distance * cos(pitchRad) * cos(yawRad);
        Position.y = Target.y + Distance * sin(pitchRad);
        Position.z = Target.z + Distance * cos(pitchRad) * sin(yawRad);

        Front = glm::normalize(Target - Position);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

// --------------------------------------------------
// ModelController Class
// --------------------------------------------------
const float MODEL_SPEED = 7.0f;

class ModelController {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;
    glm::vec3 WorldUp;
    float Yaw;

    float MovementSpeed;

    ModelController(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(MODEL_SPEED),
        Yaw(yaw),
        WorldUp(up)
    {
        Position = position;
        updateVectors();
    }

    glm::mat4 GetModelMatrix() const {
        return glm::translate(glm::mat4(1.0f), Position);
    }

    void SetYaw(float newYaw) {
        Yaw = newYaw;
        updateVectors();
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void updateVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw));
        front.y = 0.0f;
        front.z = sin(glm::radians(Yaw));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainYaw = false)
    {
        float sensitivity = 0.1f;
        xoffset *= sensitivity;

        Yaw += xoffset;

        if (constrainYaw)
        {
            if (Yaw > 360.0f)
                Yaw -= 360.0f;
            else if (Yaw < 0.0f)
                Yaw += 360.0f;
        }

        updateVectors();
    }

    float Zoom = 45.0f;

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

};

#endif // CAMERA_AND_MODEL_CONTROLLER_H
