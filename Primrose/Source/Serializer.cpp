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

AssetType Serializer::GetAssetTypeFromFile(const Asset& asset) const {

	std::ifstream File;
	File.open(asset.m_Path);
	if (!File.is_open()) {
		m_CoreReference->SystemLog("Failed to check asset type. Reason: Could not open file!");
		return AssetType::INVALID;
	}

	//First line is always type!
	std::string Line;
	getline(File, Line);
	const auto Type = std::stoi(Line, nullptr);
	return static_cast<AssetType>(Type);
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

		const MaterialFileHeader NewHeader;
		file << std::to_string(static_cast<uint32>(NewHeader.m_Type));
		file << "\n";
		file << std::to_string(NewHeader.m_Version);
		file << "\n";

		//TODO: Encrypt Header?

		//IMPORTANT NOTE: Currently the files are basically the data from variables back to back with a new line between each.
		// -This could be optimized further by adding all the file data bytes into 1 buffer and putting the value of it in the file.
	}
	break;
	default:
		return;
	}

}
bool Serializer::ValidateFileHeader(const Asset& asset) const {

	std::ifstream File;
	File.open(asset.m_Path);
	if (!File.is_open())
		return false;

	bool Results = false;

	//First line is always type!
	std::string Line;
	getline(File, Line);
	const auto Type = std::stoi(Line, nullptr);
	if (static_cast<AssetType>(Type) == asset.m_Type)
		Results = true;
	else {
		m_CoreReference->SystemLog("Discrepancy in asset type is detected between file and asset entry.");
		Results = false;
	}

	//Version line comes second!
	Line.clear();
	getline(File, Line);
	const auto Version = std::stoi(Line, nullptr);
	if (Version == AssetFilesVersions::MATERIAL_FILE_VERSION)
		Results = true;
	else {
		m_CoreReference->SystemLog("Discrepancy in asset version is detected between file and asset entry.");
		Results = false;
	}

	File.close();
	return Results;
}