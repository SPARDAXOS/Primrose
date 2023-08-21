#include "Systems/ModelStorage.hpp"
#include "Systems/Core.hpp"
#include "Asset.hpp"


ModelStorage::ModelStorage(Core& core) noexcept 
	:	m_Core(&core)
{
	m_TextureStorage = core.GetTextureStorage();
}
ModelStorage::~ModelStorage() {

	if (!m_ModelsStorage.empty()) {
		auto CleanUp = [](Model* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_ModelsStorage), std::end(m_ModelsStorage), CleanUp);
		m_ModelsStorage.clear();
	}
}

bool ModelStorage::LoadModel(Asset& asset) {

	Assimp::Importer Importer;
	unsigned int Flags = 0;
	Flags |= aiProcess_FlipUVs;
	Flags |= aiProcess_Triangulate;


	const aiScene* Scene = Importer.ReadFile(asset.m_Path.string(), Flags);
	if (Scene == nullptr || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
		m_Core->SystemLog("Error loading model [" + asset.m_Name + "] at " + asset.m_Path.string());
		m_Core->SystemLog(std::string("ModelModel error: ") + Importer.GetErrorString());
		return false;
	}

	Model* NewModel = new Model();
	ProcessNode(*NewModel, Scene->mRootNode, Scene, asset.m_EditorAsset); //TODO: Kinda Sketchy when it comes to detecting errors.
	//-Instead save an error message during the recursion and check if there are any errors here after the recursion before returning true/false
	//TODO: Delete model if any errors occured in the recursion. Otherwise, add it to models storage and return true!
	m_ModelsStorage.emplace_back(NewModel);

	return true;
}

void ModelStorage::ProcessNode(Model& model, aiNode* node, const aiScene* scene, bool editorAsset) {

	//Loop through mesh indices in each node and add them to model.
	for (uint32 index = 0; index < node->mNumMeshes; index++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[index]];
		ProcessMesh(model, mesh, scene, editorAsset);
	}

	//Recursively do the same for every child node of this node.
	for (uint32 index = 0; index < node->mNumChildren; index++) {
		ProcessNode(model, node->mChildren[index], scene, editorAsset);
	}
}
void ModelStorage::ProcessMesh(Model& model, aiMesh* mesh, const aiScene* scene, bool editorAsset) {

	std::vector<Vertex> Vertices;
	std::vector<uint32> Indicies;
	std::vector<Texture2D*> Textures;

	//TODO: I could split each step into its own function.

	//Vertices
	for (uint32 index = 0; index < mesh->mNumVertices; index++) {
		Vertex NewVertex;

		//Position
		NewVertex.m_Position.m_X = mesh->mVertices[index].x;
		NewVertex.m_Position.m_Y = mesh->mVertices[index].y;
		NewVertex.m_Position.m_Z = mesh->mVertices[index].z;

		//UV - Assimp allows a model to have up to 8 different uvs per vertex. Currently using the first set only [0].
		if (mesh->mTextureCoords[0] != nullptr) {
			NewVertex.m_TextureCoordinates.m_X = mesh->mTextureCoords[0][index].x;
			NewVertex.m_TextureCoordinates.m_Y = mesh->mTextureCoords[0][index].y;
		}
		else {
			NewVertex.m_TextureCoordinates.m_X = 0.0f;
			NewVertex.m_TextureCoordinates.m_Y = 0.0f;
		}
		
		//Normal
		NewVertex.m_Normal.m_X = mesh->mNormals->x;
		NewVertex.m_Normal.m_Y = mesh->mNormals->y;
		NewVertex.m_Normal.m_Z = mesh->mNormals->z;


		Vertices.push_back(NewVertex);
		//NewMesh->AddVertex(NewVertex);
	}

	//Indices
	for (uint32 i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (uint32 j = 0; j < face.mNumIndices; j++)
			Indicies.push_back(face.mIndices[j]);
			//NewMesh->AddIndex(face.mIndices[j]);
	}

	//Textures
	//LoadMeshTextures(*NewMesh, mesh, scene, editorAsset);
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//You do this for each type to get all textures - TODO: Could be made into a function where you pass type?
		for (uint32 i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++) {
			aiString TextureName;
			material->GetTexture(aiTextureType_DIFFUSE, i, &TextureName);
			Texture2D* Texture = nullptr;
			if (!m_TextureStorage->GetTexture2DByName(TextureName.C_Str(), Texture)) {
				m_Core->SystemLog(std::string("Failed to find texture ") + TextureName.C_Str() + " for model."); //Bad message
				continue;
				//Maybe its not the end of the world if one texture was not loaded?
			}
			Textures.push_back(Texture);
			//targetMesh.AddTexture(Texture);
		}

	}


	Mesh* NewMesh = new Mesh(*mesh, Vertices, Indicies, Textures);
	model.AddMesh(NewMesh);
}
void ModelStorage::LoadMeshTextures(Mesh& targetMesh, aiMesh* mesh, const aiScene* scene, bool editorAsset) {

	//Loads textures and calls AssetManager to create assets for them. Own asset type? 
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//You do this for each type to get all textures - TODO: Could be made into a function where you pass type?
		for (uint32 i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++) {
			aiString TextureName;
			material->GetTexture(aiTextureType_DIFFUSE, i, &TextureName);
			Texture2D* Texture = nullptr;
			if (!m_TextureStorage->GetTexture2DByName(TextureName.C_Str(), Texture)) {
				m_Core->SystemLog(std::string("Failed to find texture ") + TextureName.C_Str() + " for model."); //Bad message
				//Maybe its not the end of the world if one texture was not loaded?
			}

			//targetMesh.AddTexture(Texture);
		}




	}
}