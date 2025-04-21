#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <GL/glew.h>  // O glad dependiendo de lo que estés usando
#include <glm/glm.hpp>

class ShaderLoader {
public:
    // Función para cargar y compilar los shaders
    static GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath);

private:
    // Función para leer el contenido de un archivo de texto (por ejemplo, el código del shader)
    static std::string readFile(const std::string& filePath);

    // Función para compilar el código del shader
    static GLuint compileShader(const std::string& code, GLenum type);

    // Función para vincular los shaders a un programa
    static GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif
