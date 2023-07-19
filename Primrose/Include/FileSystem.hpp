#pragma once
#include <filesystem>
#include <string_view>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Utility.hpp"


class AssetPath final {//Maybe change name to directory?
	//Contains a folder path and a list of all Assets in there

	std::filesystem::path m_ParentPath;
	//std::vector<Asset<int>> m_Files; //This is going to be a problem - Maybe devide them into separate vectors based on type? It might not be redundant since each will
	//be sent to the appropriate system for loading so im saving myself the trouble of reiterating. Unless, i go through all one by one and depending on type i load 
	//using the appropriate system
	std::vector<std::filesystem::path> m_Folders;

	//After finish loading up all the paths, i go through all the assets and load them using the paths saved in the asset file
};

//Maybe make some asset hierarchy? texture2D ineheriting from AssetBase
template<class AssetType>
class Asset final {
	//Contains a file path and a pointer to that loaded asset.
	Asset() = delete;
	Asset(std::filesystem::path path)
		: m_AssetPath(path)
	{
	}

	std::filesystem::path m_AssetPath;
	AssetType* m_Data;
};



class FileSystem final {
public:

	explicit FileSystem() noexcept = default;
	~FileSystem() {

	}

	FileSystem(const FileSystem&) = delete;
	FileSystem& operator=(const FileSystem&) = delete;

	FileSystem(FileSystem&&) = delete;
	FileSystem& operator=(FileSystem&&) = delete;

public:
	bool LoadProjectFiles() {

		if (!CheckForContentDirectory())
			return false;

		for (auto& directory : std::filesystem::directory_iterator(std::filesystem::current_path())) {
			if (directory.path().filename() == "Content") {
				CheckDirectory(directory);
			}
		}

		return true;
	}


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





private:
	bool CheckForContentDirectory() {

		if (std::filesystem::exists("Content")) {
			//TODO: Implement proper error handling for this class
			PrintMessage("Content folder was found!");
			return true;
		}

		return false;
	}
	void CheckDirectory(std::filesystem::path path) {

		for (auto& directory : std::filesystem::directory_iterator(path)) {
			if (directory.is_regular_file()) {
				std::cout << "File at: " << directory.path() << std::endl;
				m_FilePaths.push_back(directory);
			}
			else {
				std::cout << "Folder at: " << directory.path() << std::endl;
				m_FolderPaths.push_back(directory);
				CheckDirectory(directory);
			}
		}
	}
	


private:

	//Rethink this first. Directories list might be more applicable
	std::vector<std::filesystem::path> m_FilePaths;
	std::vector<std::filesystem::path> m_FolderPaths;
};



