#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "assimp_glm_helpers.h"
#include "animdata.h"

using namespace std;

class Model
{
public:
	// model data 
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh>    meshes;
	string directory;
	bool gammaCorrection;
	string texturePath;



	// constructor, expects a filepath to a 3D model.
	Model(const string& path, const string& texturePath)
	{
		this->texturePath = texturePath;
		loadModel(path);
	}

	void Draw(Shader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }
	/* ---------- NUEVO ---------- */
	// Devuelve el extremo mínimo y máximo de la AABB (en unidades del modelo)
	glm::vec3 GetAABBMin() const { return aabbMin; }
	glm::vec3 GetAABBMax() const { return aabbMax; }

private:

	std::map<string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;


	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}

		//  Aquí mostramos cuántas animaciones hay
		if (scene->mNumAnimations > 0) {
			std::cout << "Cantidad de animaciones: " << scene->mNumAnimations << std::endl;

			for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation* animation = scene->mAnimations[i];
				std::cout << "Animacion #" << i
					<< " - Nombre: " << animation->mName.C_Str()
					<< " - Duracion: " << animation->mDuration
					<< " - Ticks por segundo: " << animation->mTicksPerSecond
					<< std::endl;
			}
		}
		else {
			std::cout << "Este modelo no contiene animaciones." << std::endl;
			//cambiar a modelo estatico
		}

		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}



	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

	void SetVertexBoneDataToDefault(Vertex& vertex)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex.m_BoneIDs[i] = -1;
			vertex.m_Weights[i] = 0.0f;
		}
	}


	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		unsigned int uvChannel = 0;
		if (!textures_loaded.empty()) {
			uvChannel = textures_loaded[0].uvIndex; // Usa el canal de UV de la textura principal
		}

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			SetVertexBoneDataToDefault(vertex);
			vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
			vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

			if (mesh->HasTextureCoords(uvChannel) && mesh->mTextureCoords[uvChannel]) {
				vertex.TexCoords = glm::vec2(mesh->mTextureCoords[uvChannel][i].x, mesh->mTextureCoords[uvChannel][i].y);
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			aabbMin = glm::min(aabbMin, vertex.Position);
			aabbMax = glm::max(aabbMax, vertex.Position);
			vertices.push_back(vertex);
		}


		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Cargar una única textura general
		if (textures_loaded.empty())
		{
			Texture texture;
			texture.id = TextureFromFile(texturePath.c_str(), "fgghf"); // es para asignar la textura apartir de la direccion pero solo maneja una aparentemente
			texture.type = "jghg";///cambiar a modo de iluminacion de la textura
			texture.path = texturePath;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
		else
		{
			textures.push_back(textures_loaded[0]);
		}

		ExtractBoneWeightForVertices(vertices, mesh, scene);

		return Mesh(vertices, indices, textures);
	}


	void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			if (vertex.m_BoneIDs[i] < 0)
			{
				vertex.m_Weights[i] = weight;
				vertex.m_BoneIDs[i] = boneID;
				break;
			}
		}
	}


	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		auto& boneInfoMap = m_BoneInfoMap;
		int& boneCount = m_BoneCounter;

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}

	/* ---------- NUEVO ---------- */
	glm::vec3 aabbMin{ FLT_MAX,  FLT_MAX,  FLT_MAX };
	glm::vec3 aabbMax{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	/* ---------- Métodos auxiliares ---------- */
	void updateAABB(const glm::vec3& v) {
		aabbMin = glm::min(aabbMin, v);
		aabbMax = glm::max(aabbMax, v);
	}


	unsigned int TextureFromFile(const char* path, const string&, bool gamma = false)
	{
		string filename = string(path); // No concatenamos con directory
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << filename << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}


	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			unsigned int uvIndex = 0;
			mat->GetTexture(type, i, &str, nullptr, &uvIndex); // Obtener el índice de UV

			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				texture.uvIndex = uvIndex; // <-- Añade este campo a tu struct Texture
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}


};



#endif
