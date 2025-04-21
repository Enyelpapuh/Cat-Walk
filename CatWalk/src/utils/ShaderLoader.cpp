#include "ShaderLoader.h"  
#include <fstream>  
#include <sstream>  
#include <iostream>  

ShaderLoader::ShaderLoader(const std::string& vertexPath, const std::string& fragmentPath) {  
   std::string vertexSource = readFile(vertexPath);  
   std::string fragmentSource = readFile(fragmentPath);  

   GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);  
   GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);  

   shaderID = linkProgram(vertexShader, fragmentShader);  

   // Eliminar shaders después de enlazarlos al programa
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
}  

GLuint ShaderLoader::compileShader(const std::string& source, GLenum type) {  
   GLuint shader = glCreateShader(type);  
   const char* sourceCStr = source.c_str();  
   glShaderSource(shader, 1, &sourceCStr, nullptr);  
   glCompileShader(shader);  

   // Verificar si hubo errores de compilación  
   GLint success;  
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);  
   if (!success) {  
       char infoLog[512];  
       glGetShaderInfoLog(shader, 512, nullptr, infoLog);  
       std::cerr << "Error al compilar el shader: " << infoLog << std::endl;  
   }  

   return shader;  
}  

GLuint ShaderLoader::linkProgram(GLuint vertexShader, GLuint fragmentShader) {  
   GLuint program = glCreateProgram();  
   glAttachShader(program, vertexShader);  
   glAttachShader(program, fragmentShader);  
   glLinkProgram(program);  

   // Verificar si hubo errores de enlace  
   GLint success;  
   glGetProgramiv(program, GL_LINK_STATUS, &success);  
   if (!success) {  
       char infoLog[512];  
       glGetProgramInfoLog(program, 512, nullptr, infoLog);  
       std::cerr << "Error al enlazar el programa de shaders: " << infoLog << std::endl;  
   }  

   return program;  
}  

std::string ShaderLoader::readFile(const std::string& filePath) {  
   std::ifstream file(filePath);  
   if (!file.is_open()) {  
       std::cerr << "No se pudo abrir el archivo: " << filePath << std::endl;  
       return "";  
   }  
   std::stringstream buffer;  
   buffer << file.rdbuf();  
   return buffer.str();  
}  

void ShaderLoader::use() {  
   glUseProgram(shaderID);  
}  

GLuint ShaderLoader::getID() const {  
   return shaderID;  
}
