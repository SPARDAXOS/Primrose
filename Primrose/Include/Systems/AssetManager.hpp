#pragma once
#include <filesystem>
#include <string_view>
#include <sstream>
#include <fstream>

#include "Asset.hpp"
#include "Utility.hpp"




class Directory final {
public:
	Directory() = delete;
	Directory(std::filesystem::path path, std::string_view name) 
		:	m_Path(path), m_Name(name)
	{
	}

	inline bool DoesAssetExist(const std::string_view& path) const {
		for (uint32 index = 0; index < m_Assets.size(); index++) {
			if (m_Assets.at(index)->m_Path == path)
				return true;
		}
		return false;
	}
	inline bool DoesFolderExist(const std::string_view& path) const {
		for (uint32 index = 0; index < m_Folders.size(); index++) {
			if (m_Folders.at(index)->m_Path == path)
				return true;
		}
		return false;
	}

	std::filesystem::path m_Path;
	std::filesystem::path m_ParentPath;
	std::string m_Name;
	std::vector<Asset*> m_Assets; 
	std::vector<Directory*> m_Folders;
};


class Serializer;
class TextureStorage;
class Core;
class Material;

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
	//TODO: Template this instead!
	bool CreateAsset(AssetType type, Directory& location);
	bool CreateNewFolder(Directory& location);
	bool RemoveAsset(Asset& asset);

public:
	inline Directory* GetProjectRoot() const noexcept { return m_ProjectRoot; }
	inline Directory* GetEditorRoot() const noexcept { return m_EditorRoot; }

public:
	[[nodiscard]] inline static bool Read(const std::string_view& filePath, std::string& buffer) {

		std::stringstream TargetFileData;
		std::ifstream TargetFile(filePath.data(), std::ios::in);
		if (!TargetFile.is_open()) {
			PrintMessages("File failed to open! ", filePath.data());
			return false;
		}

		std::string Line;
		while (getline(TargetFile, Line))
		{
			TargetFileData << Line << "\n";
		}

		buffer = TargetFileData.str();
		TargetFile.close();
		return true;
	}
	[[nodiscard]] inline static bool CRead(const std::string_view& filePath, std::string& buffer) {
		
		//Untested solution that combines both methods and might be faster than both!
		//std::ifstream t("file.txt");
		//t.seekg(0, std::ios::end);
		//size_t size = t.tellg();
		//std::string buffer(size, ' ');
		//t.seekg(0);
		//t.read(&buffer[0], size);

		std::string TargetFileData;

		FILE* TargetFile;
		fopen_s(&TargetFile, filePath.data(), "r");
		if (TargetFile == nullptr) {
			PrintMessages("File failed to open! ", filePath.data());
			return false;
		}

		fseek(TargetFile, 0, SEEK_END);
		const std::size_t FileSize = ftell(TargetFile);
		fseek(TargetFile, 0, SEEK_SET);

		TargetFileData.resize(FileSize + 1); // +1 byte as padding for safety

		fread_s(TargetFileData.data(), FileSize, 1, FileSize, TargetFile);
		fclose(TargetFile);
		TargetFile = nullptr;


		buffer = TargetFileData;
		return true;
	}

	//TODO: Send extension to function along with newasset pointer to determine asset type! and set it !
	//TODO: Add opening file at spot functionality
	//TODO: Use main scene for the gameobjects heirarchy instead
	//TODO: Changing names is gonna be hard to fix cause renaming affects it and creating new gameobjects does too.

	//IMPORTANT NOTE: Turning this class into a massive storage of all asset types might not be a bad idea.
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

private:
	std::vector<Directory*> m_ProjectDirectories;
	std::vector<Directory*> m_EditorDirectories;

	std::vector<Asset*> m_ProjectTextureAssets;
	std::vector<Asset*> m_EditorTextureAssets;

	//Create one for materials
	std::vector<Asset*> m_MaterialAssets;
	std::vector<Material*> m_MaterialStorage;


	Directory* m_ProjectRoot	{ nullptr };
	Directory* m_EditorRoot		{ nullptr };


	Core* m_CoreReference;
	TextureStorage* m_TextureStorageReference;
	Serializer* m_SerializerReference;
};



