
#include "material.h"

Material::Material(ShaderProgram* shaderProgram)
{
    // Increment the reference counter on the shader program.
    shaderProgram->IncRefCount();
    m_shaderProgram = shaderProgram;
}

Material::~Material()
{
    // Free shader program
    if (m_shaderProgram != nullptr)
        m_shaderProgram->DecRefCount();

    // Free textures
    for (int i = 0; i < m_textures.size(); i++)
    {
        m_textures[i]->DecRefCount();
    }
}

void Material::SetTexture(char* name, Texture* texture)
{
    // Bind shader program
    m_shaderProgram->Bind();

    // Request uniform from shader.
    GLint uniform = glGetUniformLocation(m_shaderProgram->GetGLShaderProgram(), name);

    // If there was no uniform location, print an error and return from the function.
    if (uniform == -1)
    {
        std::cout << "Uniform: " << name << " not found in shader program." << std::endl;
        return;
    }

    texture->IncRefCount();

    // Search through current texture uniforms to find a match.
    for (int i = 0; i < m_textureUniforms.size(); i++)
    {
        // If there's a match replace the texture.
        if (m_textureUniforms[i] == uniform)
        {
            m_textures[i]->DecRefCount();
            m_textures[i] = texture;
            return;
        }
    }

    // There is no match, add the new texture.
    m_textureUniforms.push_back(uniform);
    m_textures.push_back(texture);
}

void Material::SetMatrix(char* name, glm::mat4 matrix)
{
    // Bind shader program
    m_shaderProgram->Bind();

    // Request uniform for matrix
    GLint uniform = glGetUniformLocation(m_shaderProgram->GetGLShaderProgram(), name);

    // If there was no uniform location, print an error and return from the function.
    if (uniform == -1)
    {
        std::cout << "Uniform: " << name << " not found in shader program." << std::endl;
        return;
    }

    // Search through current matrix uniforms to find a match.
    for (int i = 0; i < m_matrixUniforms.size(); i++)
    {
        // If there's a match replace the matrix.
        if (m_matrixUniforms[i] == uniform)
        {
            m_matrices[i] = matrix;
            return;
        }
    }

    // There is no match, add the new matrix.
    m_matrixUniforms.push_back(uniform);
    m_matrices.push_back(matrix);
}


void Material::Bind()
{
    m_shaderProgram->Bind();

    // Bind all textures
    for (int i = 0; i < m_textureUniforms.size(); i++)
    {
        // This enum value can be incremented to bind to different texture locations
        glActiveTexture(GL_TEXTURE0 + i);

        // Bind the texture
        glBindTexture(GL_TEXTURE_2D, m_textures[i]->GetGLTexture());

        // Use the the texture from GL_TEXTURE0 + i at the given texture uniform location.
        glUniform1i(m_textureUniforms[i], i);
    }

    // Set all matrix data
    for (int i = 0; i < m_matrixUniforms.size(); i++)
    {
        glUniformMatrix4fv(m_matrixUniforms[i], 1, GL_FALSE, &(m_matrices[i][0][0]));
    }
}

void Material::Unbind()
{
    // Unbind all owned objects.
    for (int i = 0; i < m_textureUniforms.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    m_shaderProgram->Unbind();
}
