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

	//Layout
	/*
		Texture2D* m_Diffuse;
		Texture2D* m_Ambient;
		Texture2D* m_Specular;

		float m_AmbientStrength;

		float m_SpecularStrength;
		int32 m_SpecularShininess;
	*/

	//Create or Open file
	std::ofstream File(output.GetAsset().m_Path);
	AddHeader(File, AssetType::MATERIAL);

	//This is a material and i havent decided what no texture is for materials. I want to avoid looking for NONE texture since it doesnt exist! unnecessary look up.
	if (output.m_Diffuse != nullptr)
		File << output.m_Diffuse->GetName() << "\n";
	else
		File << "None" << "\n"; //Make sure its same text as what the details menu names it if there is no sprite!

	if (output.m_Ambient != nullptr)
		File << output.m_Ambient->GetName() << "\n";
	else
		File << "None" << "\n"; //Make sure its same text as what the details menu names it if there is no sprite!

	if (output.m_Specular != nullptr)
		File << output.m_Specular->GetName() << "\n";
	else
		File << "None" << "\n"; //Make sure its same text as what the details menu names it if there is no sprite!


	File << std::to_string(output.m_AmbientStrength) << "\n";
	File << std::to_string(output.m_SpecularStrength) << "\n";
	File << std::to_string(output.m_SpecularShininess) << "\n";

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

	//Layout
	/* 
		Texture2D* m_Diffuse;
		Texture2D* m_Ambient;
		Texture2D* m_Specular;
		
		float m_AmbientStrength;
		
		float m_SpecularStrength;
		int32 m_SpecularShininess;
	*/

	const Asset* TargetAsset = &input.GetAsset();
	if (!ValidateFileHeader(*TargetAsset)) {
		m_CoreReference->SystemLog("Failed to serialize material from file [" + TargetAsset->m_Name + "] - Header validation failed.");
		return false;
	}


	std::ifstream File;
	File.open(TargetAsset->m_Path);
	if (!File.is_open()) {
		m_CoreReference->SystemLog("Failed to serialize material from file [" + TargetAsset->m_Name + "] - Could not open file.");
		return false;
	}


	//Terrible...
	//This whole thing needs to be reworked later.
	//Better way is to check how the memeory is laid out for an object.
	//Load that memory as is to a file + header
	//Get it from file and give its data to a variable or something.

	std::string CurrentLine;
	uint32 CurrentLineCount = 0;
	while (getline(File, CurrentLine)) {
		CurrentLineCount++;
		//Skip header elements
		if (CurrentLineCount == 1 || CurrentLineCount == 2)
			continue;

		if (CurrentLineCount == 3) {
			if (CurrentLine == "None")
				continue;

			Texture2D* Diffuse;
			if (m_TextureStorageReference->GetTexture2DByName(CurrentLine, Diffuse))
				input.m_Diffuse = Diffuse;
		}
		else if (CurrentLineCount == 4) {
			if (CurrentLine == "None")
				continue;

			Texture2D* Ambient;
			if (m_TextureStorageReference->GetTexture2DByName(CurrentLine, Ambient))
				input.m_Ambient = Ambient;
		}
		else if (CurrentLineCount == 5) {
			if (CurrentLine == "None")
				continue;

			Texture2D* Specular;
			if (m_TextureStorageReference->GetTexture2DByName(CurrentLine, Specular))
				input.m_Specular = Specular;
		}
		else if (CurrentLineCount == 6) {
			const auto Data = std::stof(CurrentLine, nullptr);
			input.m_AmbientStrength = Data;
		}
		else if (CurrentLineCount == 7) {
			const auto Data = std::stof(CurrentLine, nullptr);
			input.m_SpecularStrength = Data;
		}
		else if (CurrentLineCount == 8) {
			const auto Data = std::stoi(CurrentLine, nullptr);
			input.m_SpecularShininess = Data;
		}
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

	const Asset* TargetAsset = &material.GetAsset();

	std::ofstream File;

	File.open(TargetAsset->m_Path);
	AddHeader(File, TargetAsset->m_Type);
	File.close();

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

bool Serializer::TestSerializeToFile(const Material& output) const {

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
bool Serializer::TestSerializeFromFile(Material& input) const {

	//Check header!
	const Asset* TargetAsset = &input.GetAsset();
	if (!ValidateFileHeader(*TargetAsset)) {
		m_CoreReference->SystemLog("Failed to serialize material from file [" + TargetAsset->m_Name + "]");
		return false;
	}

	//NOTE: Im reading the whole file already during validation. This could be optimized away.
	//IMPORTANT NOTE: Maybe a function to read file and skip header?
	std::string Buffer;
	if (!AssetManager::CRead(TargetAsset->m_Path.string(), Buffer)) {
		return false;
	}

	//For now i just dont bother with the header. This is a temporary solution to skipping the header? not sure. Some parts could be reused here.
	char Data[512];
	char* ptr = Data;
	std::memcpy(ptr, Buffer.data() + sizeof(MaterialFileHeader), Buffer.size() - sizeof(MaterialFileHeader));

	if (!input.SerializeFromFile(ptr)) {
		//?
		return false;
	}

	return true;
}


AssetType Serializer::GetAssetTypeFromFile(const Asset& asset) const {

	std::string Buffer; //Dumping the whole file in a string is inefficient!
	if (!AssetManager::CRead(asset.m_Path.string(), Buffer)) {
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

//IMPORTNAT NOTE: Should be templated!!!!!!
bool Serializer::ValidateFileHeader(const Asset& asset) const {

	std::string Buffer;
	if (!AssetManager::CRead(asset.m_Path.string(), Buffer)) {
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