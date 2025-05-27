
#pragma once
#include "shaderProgram.h"
#include "texture.h"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

class Material
{

private:
    // Shader program
    ShaderProgram* m_shaderProgram = nullptr;

    // Texture uniforms in use.
    std::vector<GLuint> m_textureUniforms;
    // Texture objects.
    std::vector<Texture*> m_textures;

    // Uniform for matrix.
    std::vector<GLuint> m_matrixUniforms;
    // Matrices to bind with material.
    std::vector<glm::mat4> m_matrices;


public:
    // Create a material using a given shader program.
    // If you want to use a different shader program, create a new material.
    Material(ShaderProgram* shaderProgram);
    ~Material();
    void SetTexture(char* name, Texture* texture);
    void SetMatrix(char* name, glm::mat4 matrix);

    void Bind();
    void Unbind();
};