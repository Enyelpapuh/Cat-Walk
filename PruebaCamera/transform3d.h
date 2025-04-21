
#pragma once
#include "glm/gtc/matrix_transform.hpp"

class Transform3D {

private:
    float m_scale;
    glm::vec3 m_rotation;
    glm::vec3 m_position;

    // m_matrix is only calculated if matrixDirty is true.
    bool m_matrixDirty;
    bool m_inverseDirty;

    glm::mat4 m_rotationMatrix;
    glm::mat4 m_matrix;
    glm::mat4 m_inverseMatrix;

public:
    Transform3D();

    // returns the scale
    float Scale();
    // returns the rotation in radians
    glm::vec3 Rotation();
    // returns the position as a vec2
    glm::vec3 Position();

    // sets the scale
    void SetScale(float s);
    // sets the rotation (radians)
    void SetRotation(glm::vec3 r);
    // sets the position vector
    void SetPosition(glm::vec3 v);

    // increments the rotation (radians)
    void RotateX(float r);
    void RotateY(float r);
    void RotateZ(float r);

    // increments the position vector
    void Translate(glm::vec3 v);

    glm::mat4 GetMatrix();
    glm::mat4 GetInverseMatrix();
    glm::vec3 GetUp();
    glm::vec3 GetForward();
    glm::vec3 GetRight();
	glm::vec3 GetPosition();
};