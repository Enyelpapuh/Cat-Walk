#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <GL/glew.h>  // O glad dependiendo de lo que est�s usando
#include <glm/glm.hpp>

class ShaderLoader {
public:
    // Funci�n para cargar y compilar los shaders
    static GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath);

private:
    // Funci�n para leer el contenido de un archivo de texto (por ejemplo, el c�digo del shader)
    static std::string readFile(const std::string& filePath);

    // Funci�n para compilar el c�digo del shader
    static GLuint compileShader(const std::string& code, GLenum type);

    // Funci�n para vincular los shaders a un programa
    static GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif
