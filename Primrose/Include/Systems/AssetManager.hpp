#pragma once
#include "Asset.hpp"





class Serializer;
class TextureStorage;
class ModelStorage;
class Core;
class Material;
class Texture2D;

class AssetManager final {
public:

	AssetManager() = delete;
	AssetManager(Core& core) noexcept;
	~AssetManager() {
		CleanUpAssets();
		CleanUpDirectories();
	}

	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

public:
	bool LoadAssets();

public:
	//TODO: Template this instead!
	bool CreateAsset(AssetType type, Directory& location);
	bool CreateNewFolder(Directory& location);
	bool RemoveAsset(Asset& asset);
	bool RemoveDirectory(Directory& directory);

public:
	inline Directory* GetProjectRoot() const noexcept { return m_ProjectRoot; }
	inline Directory* GetEditorRoot() const noexcept { return m_EditorRoot; }


	//Temporary until ref is added to asset.
	Material* GetMaterial(const Asset& asset) const noexcept;

public:
	[[nodiscard]] bool RequestTexture2D(const std::string_view& name, Texture2D*& ptr) const noexcept;
	inline std::vector<Material*> GetMaterialsStorage() const noexcept { return m_MaterialStorage; }

	//TODO: Send extension to function along with newasset pointer to determine asset type! and set it !
	//TODO: Add opening file at spot functionality
	//TODO: Use main scene for the gameobjects heirarchy instead
	//TODO: Changing names is gonna be hard to fix cause renaming affects it and creating new gameobjects does too.

	//IMPORTANT NOTE: Turning this class into a massive storage of all CUSTOM asset types might not be a bad idea.
	//-The only weird thing would be the texture storage that loads and manages textures. However, it might not be bad since they require unique work.
	//-Think about this and decided!


private:
	bool CheckForNecessaryDirectories();
	bool SetupProjectDirectories();
	bool SetupEditorDirectories();

	void ScanDirectory(Directory& parent, bool editorDirectory = false);
	void SetupAsset(Asset& asset, bool editorAsset = false);

	bool LoadProjectAssets();
	bool LoadEditorAssets();

	bool SerializeAssets();

	void CleanUpAssets() noexcept;
	void CleanUpDirectories() noexcept;

private:
	bool CreateMaterialAssetFile(Directory& location);

	template<typename T>
	inline bool RemoveAssetEntry(Asset& asset) {}
	bool RemoveFolderEntry(Directory& folder);

	[[nodiscard]] bool RemoveMaterialFromStorage(const Asset& asset);

private:
	[[nodiscard]] bool FindProjectDirectory(const std::string_view& path, Directory*& target) const noexcept;
	[[nodiscard]] bool FindEditorDirectory(const std::string_view& path, Directory*& target) const noexcept;

private:
	std::vector<Directory*> m_ProjectDirectories;
	std::vector<Directory*> m_EditorDirectories;

	std::vector<Asset*> m_ProjectTextureAssets;
	std::vector<Asset*> m_EditorTextureAssets;

	std::vector<Asset*> m_ProjectModelAssets;
	std::vector<Asset*> m_EditorModelAssets;


	std::vector<Asset*> m_MaterialAssets; //Materials arent used by the editor.



	std::vector<Material*> m_MaterialStorage;


	Directory* m_ProjectRoot	{ nullptr };
	Directory* m_EditorRoot		{ nullptr };

private:
	Core* m_Core						{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	ModelStorage* m_ModelStorage		{ nullptr };
	Serializer* m_Serializer			{ nullptr };
};



