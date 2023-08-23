#pragma once
#include "ASSIMP/Importer.hpp"
#include "ASSIMP/scene.h"
#include "ASSIMP/postprocess.h"

#include "Model.hpp"


class Core;
class Asset;
class AssetManager;


class ModelStorage final {
public:
	ModelStorage() = delete;
	ModelStorage(Core& core) noexcept;
	~ModelStorage();

	ModelStorage(const ModelStorage&) = delete;
	ModelStorage& operator=(const ModelStorage&) = delete;

	ModelStorage(ModelStorage&&) = delete;
	ModelStorage& operator=(ModelStorage&&) = delete;

public:
	[[nodiscard]] bool LoadModel(Asset& asset);

public:
	inline std::vector<Model*> GetModelStorage() const noexcept { return m_ModelsStorage; }

	//GetModelByName
	//GetModelByPath
	// EngineModels!
	//GetAllModels

private:
	void ProcessNode(Model& model, aiNode* node, const aiScene* scene, bool editorAsset);
	void ProcessMesh(Model& model, aiMesh* mesh, const aiScene* scene, bool editorAsset);

private:
	std::vector<Model*> m_ModelsStorage;

private:
	Core* m_Core						{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
};