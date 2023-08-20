#pragma once
#include <GraphicsResources.hpp>

#include <vector>

struct aiMesh;

class Mesh final {
public:
	Mesh() = delete;
	Mesh(aiMesh& data) noexcept; //TODO: Maybe supply 3 vectors here? otherwise you create a broken mesh then fix it... Might be fine tho. nothing break really. Its empty mesh!
	~Mesh() = default;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;

public:
	//INTERNAL USE ONLY!
	inline void AddVertex(Vertex vertex) { m_Vertices.push_back(vertex); }
	//INTERNAL USE ONLY!
	inline void AddIndex(uint32 index) { m_Indices.push_back(index); }
	//INTERNAL USE ONLY!
	inline void AddTexture(Texture2D* texture) { m_Textures.push_back(texture); }


private:
	aiMesh* m_Data	{ nullptr };
	std::vector<Vertex> m_Vertices;
	std::vector<uint32> m_Indices;
	std::vector<Texture2D*> m_Textures;
};