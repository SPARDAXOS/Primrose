#include "Systems/Serializer.hpp"
#include "Systems/Core.hpp"
#include "Tools/Profiler.hpp"


Serializer::Serializer(Core& core) noexcept
	:	m_CoreReference(&core)
{
	m_TextureStorageReference = m_CoreReference->GetTextureStorage();
}


//////////
//	Serialization to file.
//////////
template<typename T>
[[nodiscard]] bool Serializer::SerializeToFile(const T& output) const {
	m_CoreReference->SystemLog("Asset type was not identified.");
	m_CoreReference->SystemLog("Failed to serialize to file");
}
template<>
bool Serializer::SerializeToFile<Material>(const Material& output) const {

	//Create or Open file
	std::ofstream File(output.GetAsset().m_Path);
	AddHeader(File, AssetType::MATERIAL);

	char Buffer[512]; //Assumes the object is 512 or less. Unused bytes are cleaned before serialization to file.
	char* ptr = Buffer;
	size_t SerializedBytes;
	if (!output.SerializeToFile(ptr, SerializedBytes)) {
		m_CoreReference->SystemLog("Failed to serialize Material [" + output.GetAsset().m_Name + "]");
		File.close();
		return false;
	}
	File.write(ptr, SerializedBytes);
	File.close();

	return true;
}




//////////
//	Serialization from file.
//////////
template<typename T>
[[nodiscard]] bool Serializer::SerializeFromFile(T& input) const {
	m_CoreReference->SystemLog("Asset type was not identified.");
	m_CoreReference->SystemLog("Failed to serialize from file");
}
template<>
bool Serializer::SerializeFromFile<Material>(Material& input) const {

	//Check header!
	const Asset* TargetAsset = &input.GetAsset();
	if (!ValidateFileHeader(*TargetAsset)) {
		m_CoreReference->SystemLog("Failed to serialize material from file [" + TargetAsset->m_Name + "]");
		return false;
	}

	//NOTE: Im reading the whole file already during validation. This could be optimized away.
	//IMPORTANT NOTE: Maybe a function to read file and skip header?
	std::string Buffer;
	if (!CRead(TargetAsset->m_Path.string(), Buffer)) {
		return false;
	}

	//For now i just dont bother with the header. This is a temporary solution to skipping the header? not sure. Some parts could be reused here.
	char Data[512];
	char* ptr = Data;
	std::memcpy(ptr, Buffer.data() + sizeof(MaterialFileHeader), Buffer.size() - sizeof(MaterialFileHeader));

	if (!input.SerializeFromFile(ptr)) {
		m_CoreReference->SystemLog("Material failed to serialize data from buffer.");
		return false;
	}

	return true;
}




//////////
//	Creating file from asset.
//////////
template<typename T>
[[nodiscard]] bool Serializer::CreateFile(const T& asset) const {
	m_CoreReference->SystemLog("Asset type was not identified.");
	m_CoreReference->SystemLog("Failed to create file.");
};
template<>
bool Serializer::CreateFile<Material>(const Material& material) const {

	//? Creating a file and serializing (to file) it are pretty much the same? 
	//I guess its because open can also create files.
	//This is also saving then?
	const Asset* TargetAsset = &material.GetAsset();
	if (!SerializeToFile(material)) {
		DeleteFile(TargetAsset->m_Path);
		return false;
	}
	return true;
}



bool Serializer::DeleteFile(const std::filesystem::path& path) const {

	const auto Results = std::remove(path.string().data());
	if (Results != -0) {
		m_CoreReference->SystemLog("Failed to delete file at " + path.string());
		return false;
	}
	return true;
}
bool Serializer::DeleteFolder(const std::filesystem::path& path) const {

	const auto Results = std::filesystem::remove(path);
	if (!Results) {
		m_CoreReference->SystemLog("Failed to delete folder at " + path.string());
		return false;
	}

	return Results;
}



AssetType Serializer::GetAssetTypeFromFile(const Asset& asset) const {

	std::string Buffer; //Dumping the whole file in a string is inefficient!
	if (!CRead(asset.m_Path.string(), Buffer)) {
		return AssetType::INVALID;
	}

	MaterialFileHeader FileMarker;
	MaterialFileHeader* ptr = &FileMarker;
	std::memcpy(ptr, Buffer.data(), sizeof(MaterialFileHeader));

	return FileMarker.m_Type;
}
void Serializer::AddHeader(std::ofstream& file, AssetType type) const {

	
	//NOTE: Data gets written the moment the file closes so make sure to close it after writing is finished.

	switch (type)
	{
	case AssetType::INVALID:
		return;
	case AssetType::TEXTURE: //Texture are stored as is instead of being an engine asset type.
		return;
	case AssetType::MATERIAL: {

		//TODO: Deal with both cases of file being opened or closed.
		if (!file.is_open())
			return;

		MaterialFileHeader NewHeader;
		MaterialFileHeader* ptr = &NewHeader;
		char Buffer[512];

		std::memcpy(Buffer, ptr, sizeof(MaterialFileHeader));
		file.write(Buffer, sizeof(MaterialFileHeader));

		//TODO: Encrypt Header?

		//IMPORTANT NOTE: Currently the files are basically the data from variables back to back with a new line between each.
		// -This could be optimized further by adding all the file data bytes into 1 buffer and putting the value of it in the file.
	}
	break;
	default:
		return;
	}

}

//IMPORTNAT NOTE: Should be templated!!!!!! Is this function even needed? Getting the asset type is enough?
bool Serializer::ValidateFileHeader(const Asset& asset) const {

	std::string Buffer;
	if (!CRead(asset.m_Path.string(), Buffer)) {
		m_CoreReference->SystemLog("Failed to open file for header validation.");
		return false;
	}

	MaterialFileHeader FileMarker;
	MaterialFileHeader* ptr = &FileMarker;
	std::memcpy(ptr, Buffer.data(), sizeof(MaterialFileHeader));

	bool Results = false;
	if (FileMarker.m_Type == asset.m_Type)
		Results = true;
	else {
		m_CoreReference->SystemLog("Discrepancy in asset type is detected between file and asset entry.");
		Results = false;
	}

	if (FileMarker.m_Version == AssetFilesVersions::MATERIAL_FILE_VERSION)
		Results = true;
	else {
		m_CoreReference->SystemLog("Discrepancy in asset version is detected between file and asset entry.");
		Results = false;
	}

	return Results;
}