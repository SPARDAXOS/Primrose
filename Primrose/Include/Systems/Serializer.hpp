#pragma once
#include <sstream>
#include <fstream>

#include "Utility.hpp"
#include "Asset.hpp"


//A header contains
// -Header Start Marker
// -Asset Type
// -Asset Version
// -Header End Marker
// -
//
//


//NOTE: Maybe its better to keep the serializer only in the asset manager if its the only thing that uses it.

class Core;
class Material;
class TextureStorage;

namespace AssetFilesVersions {
	constexpr uint32 MATERIAL_FILE_VERSION = 1111;
}


class Serializer final {
private:
	struct MaterialFileHeader {

		AssetType m_Type{ AssetType::MATERIAL }; //4 bytes
		uint32 m_Version{ AssetFilesVersions::MATERIAL_FILE_VERSION }; // 4 bytes
	};

public:
	Serializer() = delete;
	Serializer(Core& core) noexcept;
	~Serializer() = default;

	Serializer(const Serializer&) = delete;
	Serializer& operator=(const Serializer&) = delete;

	Serializer(Serializer&&) = delete;
	Serializer& operator=(Serializer&&) = delete;

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


public: //TODO: Add concepts?
	template<typename T>
	[[nodiscard]] bool SerializeToFile(const T& output) const;

	template<typename T>
	[[nodiscard]] bool SerializeFromFile(T& input) const;

	template<typename T>
	[[nodiscard]] bool CreateFile(const T& asset) const;

	bool DeleteFile(const std::filesystem::path& path) const;
	bool DeleteFolder(const std::filesystem::path& path) const;

	AssetType GetAssetTypeFromFile(const Asset& asset) const;

private:
	void AddHeader(std::ofstream& file, AssetType type) const;
	bool ValidateFileHeader(const Asset& asset) const;

private:
	Core* m_Core;
	TextureStorage* m_TextureStorage;

};