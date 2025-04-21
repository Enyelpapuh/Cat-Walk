

#include "transform3d.h"

Transform3D::Transform3D()
{
    m_scale = 1;
    m_rotation = glm::vec3();
    m_position = glm::vec3();
    m_matrix = m_inverseMatrix = glm::mat4();
}

float Transform3D::Scale()
{
    return m_scale;
}

glm::vec3 Transform3D::Rotation()
{
    return m_rotation;
}

glm::vec3 Transform3D::Position()
{
    return m_position;
}

void Transform3D::SetScale(float s)
{
    m_scale = s;
    m_matrixDirty = m_inverseDirty = true;
}

void Transform3D::SetRotation(glm::vec3 r)
{
    m_rotation = r;
    m_matrixDirty = m_inverseDirty = true;
}

void Transform3D::SetPosition(glm::vec3 v)
{
    m_position = v;
    m_matrixDirty = m_inverseDirty = true;
}

void Transform3D::RotateX(float r)
{
    m_rotation.x += r;
    m_matrixDirty = m_inverseDirty = true;
}

void Transform3D::RotateY(float r)
{
    m_rotation.y += r;
    m_matrixDirty = m_inverseDirty = true;
}

void Transform3D::RotateZ(float r)
{
    m_rotation.z += r;
    m_matrixDirty = m_inverseDirty = true;
}


void Transform3D::Translate(glm::vec3 v)
{
    m_position += v;
    m_matrixDirty = m_inverseDirty = true;
}

glm::mat4 Transform3D::GetMatrix()
{
    // If anything has changed, recalculate the matrix
    if (m_matrixDirty) {
        // scale
        glm::mat4 s = glm::mat4(
            m_scale, 0, 0, 0,
            0, m_scale, 0, 0,
            0, 0, m_scale, 0,
            0, 0, 0, 1
        );

        // rotation on the x axis
        glm::mat4 rx = glm::mat4(
            1, 0, 0, 0,
            0, cos(m_rotation.x), sin(m_rotation.x), 0,
            0, -sin(m_rotation.x), cos(m_rotation.x), 0,
            0, 0, 0, 1
        );

        // rotation on the y axis
        glm::mat4 ry = glm::mat4(
            cos(m_rotation.y), 0, sin(m_rotation.y), 0,
            0, 1, 0, 0,
            -sin(m_rotation.y), 0, cos(m_rotation.y), 0,
            0, 0, 0, 1
        );

        // rotation on the z axis
        glm::mat4 rz = glm::mat4(
            cos(m_rotation.z), sin(m_rotation.z), 0, 0,
            -sin(m_rotation.z), cos(m_rotation.z), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );

        // translation
        glm::mat4 t = glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            m_position.x, m_position.y, m_position.z, 1
        );

        // compose rotation matrix
        // roll, then yaw, then pitch
        // (multiply in reverse order)
        m_rotationMatrix = ry * rx * rz;

        // calculate world matrix
        m_matrix = t * m_rotationMatrix * s;

        m_matrixDirty = false;
    }

    //return the world matrix
    return m_matrix;
}

glm::mat4 Transform3D::GetInverseMatrix()
{
    // If anything has changed, recalculate the matrix
    if (m_inverseDirty) {

        // inverse scale just divides by scale
        glm::mat4 s = glm::mat4(
            1.f / m_scale, 0, 0, 0,
            0, 1.f / m_scale, 0, 0,
            0, 0, 1.f / m_scale, 0,
            0, 0, 0, 1
        );

        // rotation on the x axis
        glm::mat4 rx = glm::mat4(
            1, 0, 0, 0,
            0, cos(m_rotation.x), -sin(m_rotation.x), 0,
            0, sin(m_rotation.x), cos(m_rotation.x), 0,
            0, 0, 0, 1
        );

        // rotation on the y axis
        glm::mat4 ry = glm::mat4(
            cos(m_rotation.y), 0, -sin(m_rotation.y), 0,
            0, 1, 0, 0,
            sin(m_rotation.y), 0, cos(m_rotation.y), 0,
            0, 0, 0, 1
        );

        // rotation on the z axis
        glm::mat4 rz = glm::mat4(
            cos(m_rotation.z), -sin(m_rotation.z), 0, 0,
            sin(m_rotation.z), cos(m_rotation.z), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );

        // translation
        glm::mat4 t = glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -m_position.x, -m_position.y, -m_position.z, 1
        );

        // compose rotation matrix
        // pitch, then yaw, then roll
        // (multiply in reverse order)
        glm::mat4 r = rz * rx * ry;

        // calculate world matrix inverse
        m_inverseMatrix = s * r * t;

        m_inverseDirty = false;
    }

    //return the world matrix
    return m_inverseMatrix;
}

glm::vec3 Transform3D::GetUp()
{
    // Force the transform to recalculate rotation if it's dirty
    if (m_matrixDirty)
    {
        GetMatrix();
    }
    // Multiplying an up vector by rotation will give this transforms up vector
    glm::vec4 up = m_rotationMatrix * glm::vec4(0, 1, 0, 1);

    return glm::vec3(up);
}

glm::vec3 Transform3D::GetForward()
{
    // Force the transform to recalculate rotation if it's dirty
    if (m_matrixDirty)
    {
        GetMatrix();
    }

    glm::vec4 forward = m_rotationMatrix * glm::vec4(0, 0, -1, 1);

    return glm::vec3(forward);
}

glm::vec3 Transform3D::GetRight()
{
    // Force the transform to recalculate rotation if it's dirty
    if (m_matrixDirty)
    {
        GetMatrix();
    }

    glm::vec4 right = m_rotationMatrix * glm::vec4(1, 0, 0, 1);

    return glm::vec3(right);
}
glm::vec3 Transform3D::GetPosition()
{
	return m_position;
}