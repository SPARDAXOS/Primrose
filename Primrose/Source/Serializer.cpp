#include "Systems/Serializer.hpp"
#include "Systems/Core.hpp"
#include "Tools/Profiler.hpp"


Serializer::Serializer(Core& core)
	:	m_CoreReference(&core)
{
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

	//Used Layout
	// 
	//Texture2D* m_Diffuse;
	//Texture2D* m_Ambient;
	//Texture2D* m_Specular;
	//
	//float m_AmbientStrength;
	//
	//float m_SpecularStrength;
	//int32 m_SpecularShininess;
	//

	std::ofstream File(output.GetAsset()->m_Path);
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

	//When it comes to deleting and such. There is a bunch of problems that would need to be tackled such as
	//What if i delete an asset from windows while the engine is open? and edge cases like this.


	return false;
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

	const Asset* TargetAsset = material.GetAsset();

	std::ofstream File;

	File.open(TargetAsset->m_Path);
	AddHeader(File, TargetAsset->m_Type);
	File.close();

	if (!SerializeToFile(material)) {
		DeleteFile(*TargetAsset);
		return false;
	}
	return true;
}


bool Serializer::DeleteFile(const Asset& asset) const {
	return std::remove(asset.m_Path.string().data()); //Error is -1 and it would turn into false(0)
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