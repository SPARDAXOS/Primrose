#pragma once
#include "ASSIMP/Importer.hpp"
#include "ASSIMP/scene.h"
#include "ASSIMP/postprocess.h"

#include "Model.hpp"


class Core;
class Asset;
class AssetManager;


class ModelLoader final {
public:
	ModelLoader() = delete;
	ModelLoader(Core& core) noexcept;
	~ModelLoader() = default;

	ModelLoader(const ModelLoader&) = delete;
	ModelLoader& operator=(const ModelLoader&) = delete;

	ModelLoader(ModelLoader&&) = delete;
	ModelLoader& operator=(ModelLoader&&) = delete;


public:
	[[nodiscard]] bool LoadModel(Asset& asset);

	//GetModelByName
	//GetModelByPath
	// EngineModels!
	//GetAllModels

private:
	void ProcessNode(Model& model, aiNode* node, const aiScene* scene, bool editorAsset);
	void ProcessMesh(Model& model, aiMesh* mesh, const aiScene* scene, bool editorAsset);
	void LoadMeshTextures(Mesh& model, aiMesh* mesh, const aiScene* scene, bool editorAsset);

private:
	std::vector<Model*> m_ModelsStorage;

private:
	Core* m_Core					{ nullptr };
	AssetManager* m_AssetManager	{ nullptr };
};