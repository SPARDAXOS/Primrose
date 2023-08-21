#include "Mesh.hpp"
#include "ASSIMP/scene.h"

Mesh::Mesh(aiMesh& data, std::vector<Vertex> vertices, std::vector<uint32> indicies, std::vector<Texture2D*> textures)
	:	m_Source(&data), m_Vertices(vertices), m_Indices(indicies), m_Textures(textures)
{
	m_VAO = new VAO;
	m_VAO->Bind();

	m_VBO = new VBO(&m_Vertices[0], static_cast<GLuint>(m_Vertices.size() * sizeof(Vertex)));
	m_EBO = new EBO(&m_Indices[0], static_cast<GLuint>(m_Indices.size() * sizeof(uint32)), static_cast<GLuint>(m_Indices.size()));
	m_VBO->Bind();
	m_EBO->Bind();

	m_VAO->Unbind();
	m_VBO->Unbind();
	m_EBO->Unbind();
}
Mesh::~Mesh() {
	delete m_VAO;
	delete m_VBO;
	delete m_EBO;
}
