#pragma once
#include <filesystem>
#include <string_view>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Utility.hpp"
#include "Profiler.hpp"


enum class AssetType {
	INVALID = 0,
	TEXTURE
};

//Maybe make some asset hierarchy? texture2D ineheriting from AssetBase
class Asset {
public:
	Asset() = default;
	Asset(std::filesystem::path path)
		: m_Path(path)
	{
	}
	Asset(std::filesystem::path path, AssetType type)
		: m_Path(path), m_Type(type)
	{
	}

	~Asset() = default;

	Asset(const Asset& other) {
		*this = other;
	}
	Asset& operator=(const Asset& other) {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Path = other.m_Path;
			this->m_Type = other.m_Type;
			return *this;
		}
	}

	Asset(Asset&& other) {
		*this = std::move(other);
	}
	Asset& operator=(Asset&& other) {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Path = std::move(other.m_Path);
			this->m_Type = std::move(other.m_Type);
			return *this;
		}
	}

	bool operator==(const Asset& other) const noexcept {
		if (m_Path != other.m_Path)
			return false;
		else if (m_Type != other.m_Type)
			return false;
		else
			return true;
	}
	bool operator!=(const Asset& other) const noexcept {
		if (!(*this == other))
			return true;
		else
			return false;
	}

public:
	std::filesystem::path m_Path;
	AssetType m_Type{ AssetType::INVALID };
};

class Directory final {//Maybe change name to directory?
public:
	Directory() = delete;
	Directory(std::filesystem::path path) 
		:	m_Path(path)
	{
	}




	//Contains a folder path and a list of all Assets in there

	std::filesystem::path m_Path;
	std::filesystem::path m_ParentPath; //This might not be needed actually. At least not for constructing the hierarchy
	std::vector<Asset*> m_Assets; //This is going to be a problem - Maybe devide them into separate vectors based on type? It might not be redundant since each will
	//be sent to the appropriate system for loading so im saving myself the trouble of reiterating. Unless, i go through all one by one and depending on type i load 
	//using the appropriate system
	std::vector<Directory*> m_Folders;

	//After finish loading up all the paths, i go through all the assets and load them using the paths saved in the asset file
};


class TextureStorage;

class AssetManager final {
public:

	AssetManager() = delete;
	AssetManager(TextureStorage& textureStorage) 
		:	m_TextureStorageReference(&textureStorage)
	{
	}
	~AssetManager() {

	}

	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

public:
	bool LoadProjectFiles();


public:
	inline Directory* GetRoot() const noexcept { return m_Root; }

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
	//NOTE: There is a lot of clean up and such that could be done in these recent classes ive made



private:
	bool CheckForContentDirectory();
	bool ScanForContent();
	void ScanDirectory(std::filesystem::path path, Directory& parent);
	void CheckAssetType(Asset& asset);
	bool LoadAssets();
	

private:

	//Rethink this first. Directories list might be more applicable

	Profiler LoadingProfiler;

	std::vector<Directory*> m_Directories;
	Directory* m_Root;

	//Since they will be cleaned up from here
	//Copies on the stack might be better 
	std::vector<Asset*> m_TexturesAssets;

	TextureStorage* m_TextureStorageReference;
};



