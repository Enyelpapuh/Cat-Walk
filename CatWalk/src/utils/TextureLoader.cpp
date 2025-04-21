// TextureLoader.cpp
#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // Asegúrate de que este archivo esté presente

#include <iostream>

Texture TextureLoader::loadTexture(const std::string& path) {
    Texture texture;

    // Cargar la imagen usando stb_image
    unsigned char* data = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channels, 0);
    if (!data) {
        std::cerr << "Error al cargar la textura: " << path << std::endl;
        texture.id = 0;
        return texture;
    }

    // Generar la textura en OpenGL
    glGenTextures(1, &texture.id);  // Generar el ID de la textura
    glBindTexture(GL_TEXTURE_2D, texture.id);  // Vincular la textura

    // Configuración de parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Establecer el formato según los canales de la imagen
    GLenum format = (texture.channels == 3) ? GL_RGB : GL_RGBA;

    // Cargar la imagen como textura
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);  // Generar mipmaps para la textura

    // Liberar los datos de la imagen
    stbi_image_free(data);

    return texture;
}
