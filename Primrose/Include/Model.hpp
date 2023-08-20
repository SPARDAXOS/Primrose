#pragma once
#include "Mesh.hpp"


class Model final {
public:
	Model() = default;
	~Model() = default;

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	Model(Model&&) = delete;
	Model& operator=(Model&&) = delete;

public:
	//Internal Use Only
	inline void AddMesh(Mesh* mesh) { m_Meshes.push_back(mesh); }



private:
	std::vector<Mesh*> m_Meshes;
};