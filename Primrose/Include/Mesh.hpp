#pragma once
#include <GraphicsResources.hpp>

#include <vector>

struct aiMesh;

class Mesh final {
public:
	Mesh() = delete;
	Mesh(aiMesh& data, std::vector<Vertex> vertices, std::vector<uint32> indicies, std::vector<Texture2D*> textures);
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;

public:

	//NOTE: Dont showcase the mdl file? in the content

public:
	VAO* m_VAO  { nullptr };
	VBO* m_VBO  { nullptr };
	EBO* m_EBO	{ nullptr };

public:
	aiMesh* m_Source	{ nullptr };
	std::vector<Vertex> m_Vertices;
	std::vector<uint32> m_Indices;
	std::vector<Texture2D*> m_Textures;
};