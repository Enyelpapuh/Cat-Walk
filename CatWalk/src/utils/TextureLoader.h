#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <string>
#include <glad/glad.h>
#include <stb_image.h>  // Asegúrate de tener esta biblioteca para cargar imágenes

// Estructura para manejar texturas
struct Texture {
    unsigned int ID;
    std::string type;
    std::string path;

    Texture() : ID(0) {}
};

class TextureLoader {
public:
    static Texture loadTexture(const std::string& path);
};

#endif
