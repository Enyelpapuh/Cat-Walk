#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <GL/glew.h>

struct Vertex {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat texCoords[2];
};

class ModelLoader {
public:
    ModelLoader(const std::string& path);
    void render() const;
    bool isLoaded() const { return modelLoaded; } // Método para chequear si el modelo fue cargado correctamente

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    bool modelLoaded; // Variable para indicar si el modelo fue cargado correctamente

    bool loadModel(const std::string& path); // Cambiar el tipo de retorno de void a bool
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif
