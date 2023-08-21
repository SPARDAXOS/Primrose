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
	//Internal Use Only - TODO: Maybe do the same thing and construct this by passing a vec in the ctor
	inline void AddMesh(Mesh* mesh) { m_Meshes.push_back(mesh); }

	inline std::vector<Mesh*> GetMeshes() const noexcept { return m_Meshes; }


private:
	std::vector<Mesh*> m_Meshes;
};