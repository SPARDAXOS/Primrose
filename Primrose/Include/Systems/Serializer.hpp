#pragma once

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

public: //TODO: Add concepts?
	template<typename T>
	[[nodiscard]] bool SerializeToFile(const T& output) const;

	template<typename T>
	[[nodiscard]] bool SerializeFromFile(T& input) const;

	template<typename T>
	[[nodiscard]] bool CreateFile(const T& asset) const;

	bool DeleteFile(const std::filesystem::path& path) const;

	AssetType GetAssetTypeFromFile(const Asset& asset) const;

private:
	void AddHeader(std::ofstream& file, AssetType type) const;
	bool ValidateFileHeader(const Asset& asset) const;

private:
	Core* m_CoreReference;
	TextureStorage* m_TextureStorageReference;

};