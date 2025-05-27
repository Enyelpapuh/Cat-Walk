

#include "mesh.h"

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

Mesh::Mesh(std::vector<Vertex3dUVNormal> vertices, std::vector<unsigned short> indices)
{
	m_vertices = vertices;
	m_indices = indices;
	// Create the shape by setting up buffers

	// Set up vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex3dUVNormal), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set up index buffer
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Mesh(std::string filePath)
{
	// before we do anything, lets first check if the file even exists:
	std::ifstream file(filePath);

	if (!file.good())
	{
		// If we encounter an error, print a message and return.
		std::cout << "Can't read file: " << filePath << std::endl;
		return;
	}

	// create variables
	Assimp::Importer importer;
	const aiScene* scene = NULL;
	unsigned int n = 0, t;

	// Load the file into a "scene"
	for (int k = 0; k < 1; k++)
		scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_PreTransformVertices);

	// only one mesh in the file, because its a simple tutorial
	const struct aiMesh* mesh = scene->mMeshes[0];

	// make the vertex buffer
	for (t = 0; t < mesh->mNumVertices; ++t)
	{
		// make a vertex
		Vertex3dUVNormal v;

		// copy from arrays to each vertex
		memcpy(&v.m_position, &mesh->mVertices[t], sizeof(glm::vec3));
		memcpy(&v.m_normal, &mesh->mNormals[t], sizeof(glm::vec3));
		memcpy(&v.m_texCoord, &mesh->mTextureCoords[0][t], sizeof(glm::vec2));

		// add to vertex buffer
		m_vertices.push_back(v);
	}

	// make the index buffer
	for (t = 0; t < mesh->mNumFaces; ++t)
	{
		const struct aiFace* face = &mesh->mFaces[t];

		// add the first 3 indices of the face to our index collection to form a triangle
		for (int i = 0; i < 3; i++)
		{
			m_indices.push_back(face->mIndices[i]);
		}

		// the face is a quad, add 3 more vertices to form a triangle
		if (face->mNumIndices == 4)
		{
			m_indices.push_back(face->mIndices[0]);
			m_indices.push_back(face->mIndices[2]);
			m_indices.push_back(face->mIndices[3]);
		}
	}

	printf("%d %d\n", (int)m_vertices.size(), (int)m_indices.size());

	// create buffers for opengl just like normal

	// Set up vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex3dUVNormal), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set up index buffer
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned short), &m_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
	// Clear buffers for the shape object when done using them.
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
}

// This macro will help us make the attribute pointers
// position, size, type, struct, element
#define SetupAttribute(index, size, type, structure, element) \
	glVertexAttribPointer(index, size, type, 0, sizeof(structure), (void*)offsetof(structure, element)); \

void Mesh::Draw()
{
	// Previously, we multiplied each vertex one by one, but now we just have to send the world matrix to the gpu.

	// Bind Vertex Buffer and Index Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	// Setup Vertex Attributes
	SetupAttribute(0, 3, GL_FLOAT, Vertex3dUVNormal, m_position);
	SetupAttribute(1, 2, GL_FLOAT, Vertex3dUVNormal, m_texCoord);
	SetupAttribute(2, 3, GL_FLOAT, Vertex3dUVNormal, m_normal);

	// Enable all attrubutes
	for (int i = 0; i < 3; i++)
		glEnableVertexAttribArray(i);

	// Draw all indices in the index buffer
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, (void*)0);

	// Unbind Vertex Buffer and Index Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Disable all attributes (enabled in the macro)
	for (int i = 0; i < 3; i++)
		glDisableVertexAttribArray(i);

}
