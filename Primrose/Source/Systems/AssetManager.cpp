#include "Systems/AssetManager.hpp"
#include "Systems/Core.hpp"



//NOTE: There is some includes weirdness going on cause texture storage had implemented filesystem before it was reworked
//TextureStorage needs the Asset class and the Loading functions from here
//This class needs a ref to TextureStorage for loading the assets and needs the class Asset
//Asset should be in its own header! Maybe along with others like texture 2d and such


AssetManager::AssetManager(Core& core) noexcept
	: m_CoreReference(&core)
{
	m_TextureStorageReference = m_CoreReference->GetTextureStorage();
}

bool AssetManager::LoadAssets() {


	//TODO: Implement proper error handling for this class
	//TODO: Log system editor messages
	//TODO: Add protection so this function can only be called once!

	if (!CheckForNecessaryDirectories())
		return false;

	if (!SetupEditorDirectories())
		return false;

	if (!LoadEditorAssets())
		return false;

	if (!SetupProjectDirectories())
		return false;

	if (!LoadProjectAssets())
		return false;

	m_CoreReference->SystemLog("AssetManager finished loading assets successfully!");
	return true;
}
bool AssetManager::CreateAsset(AssetType type, Directory& location) {
	
	
	//Get specific header from function

	//CreateFile at location with x name
	//Add it to all lists and update location lists!

	Asset* NewAsset = new Asset;
	NewAsset->m_EditorAsset = false;
	NewAsset->m_Extension = ".rose"; //Depending on AssetType? or just general engine asset
	NewAsset->m_Type = type;
	NewAsset->m_Name = "NewMaterial";
	NewAsset->m_Path = location.m_Path.string() + std::string("\\" + NewAsset->m_Name + NewAsset->m_Extension);

	if (m_MaterialAssets.size() >= 1) {
		m_CoreReference->DebugLog("Exists!" + NewAsset->m_Name);
	}

	int CopyNameIncrement = 0;
	while (location.DoesAssetExist(NewAsset->m_Path.string())) {
		CopyNameIncrement++;
		NewAsset->m_Name = "NewMaterial" + std::to_string(CopyNameIncrement);
		NewAsset->m_Path = location.m_Path.string() + std::string("\\" + NewAsset->m_Name + NewAsset->m_Extension);
	}

	Material* NewMaterial = new Material(*NewAsset);

	//Add to assets list
	m_MaterialAssets.emplace_back(NewMaterial);

	//Add to directory assets list
	location.m_Assets.emplace_back(NewAsset);



	std::string TempHeader = "##Material";
	std::ofstream File;
	//IMPORTANT NOTE: Using Windows slashes but the function does work regardless. However, using double slashes for consistency in Asset.m_Path and Directory.m_Path 
	auto Result = location.m_Path.string() + "\\" + NewAsset->m_Name + NewAsset->m_Extension;
	File.open(location.m_Path.string() + "\\" + NewAsset->m_Name + NewAsset->m_Extension);
	
	//TODO: Add header to file Function!
	File << TempHeader;

	File.close();
	return true;
}
bool AssetManager::CreateNewFolder(Directory& location) {
	
	std::string FolderName = "NewFolder";
	std::string FolderPath(location.m_Path.string() + "\\" + FolderName);

	int CopyNameIncrement = 0;
	while (location.DoesFolderExist(FolderPath)) {
		CopyNameIncrement++;
		FolderName = "NewFolder" + std::to_string(CopyNameIncrement);
		FolderPath = location.m_Path.string() + "\\" + FolderName;
	}

	if (!std::filesystem::create_directory(FolderPath))
		return false;

	Directory* NewDirectory = new Directory(FolderPath.data(), FolderName);
	m_ProjectDirectories.emplace_back(NewDirectory);
	location.m_Folders.emplace_back(NewDirectory);
	NewDirectory->m_ParentPath = location.m_Path;

	return true;
}

bool AssetManager::SerializeMaterialToFile(Material& material) {

	//NOTE: If you have a material, it means a file does exist!
	
	Asset* TargetAsset = material.GetAsset();
	auto FilePath = TargetAsset->m_Path;
	if (FilePath == "") {
		m_CoreReference->SystemLog("Invalid asset path while serializing material to file [" + TargetAsset->m_Name + "]");
		return false;
	}


	std::ofstream File;
	File.open(FilePath);
	File.clear(); //It clears the file fully before rewriting. This might be bad?
	AddAssetFileHeader(File, AssetType::MATERIAL);


}

void AssetManager::AddAssetFileHeader(std::ofstream& file, AssetType type) const noexcept {

	switch (type)
	{
	case AssetType::INVALID:
		return;
	case AssetType::TEXTURE: //Texture are stored as is instead of being an engine asset type.
		return;
	case AssetType::MATERIAL: {

	}
		break;
	default:
		return;
	}

}


bool AssetManager::CheckForNecessaryDirectories() {

	bool ContentStatus = false;
	bool EditorStatus = false;

	for (auto& DirectoryPath : std::filesystem::directory_iterator(std::filesystem::current_path())) {
		if (DirectoryPath.path().filename() == "Content") {

			Directory* NewDirectory = new Directory(DirectoryPath.path(), DirectoryPath.path().filename().string());
			m_ProjectRoot = NewDirectory;
			m_ProjectDirectories.emplace_back(NewDirectory);

			m_CoreReference->SystemLog("Content folder was located!");
			ContentStatus = true;
		}
		else if (DirectoryPath.path().filename() == "Editor") {

			Directory* NewDirectory = new Directory(DirectoryPath.path(), DirectoryPath.path().filename().string());
			m_EditorRoot = NewDirectory;
			m_EditorDirectories.emplace_back(m_EditorRoot);

			m_CoreReference->SystemLog("Editor folder was located!");
			EditorStatus = true;
		}
	}

	//TODO: Implement proper error handling for this class
	bool Results = true;
	if (!ContentStatus) {
		m_CoreReference->SystemLog("Content folder was not located!");
		Results = false;
	}
	if (!EditorStatus) {
		m_CoreReference->SystemLog("Editor folder was not located!");
		Results = false;
	}

	return Results;
}
bool AssetManager::SetupProjectDirectories() {

	if (m_ProjectRoot != nullptr) {
		m_CoreReference->SystemLog("Started scanning for project assets to load...");
		ScanDirectory(*m_ProjectRoot, false); //Sets up tree and Connections + Assets in each directory
		m_CoreReference->SystemLog("Finished scanning for project assets to load.");
		//NOTE: The tree and the directories keeping track of assets is good for reloading features since i just update a specific directory!
		//TODO: Implement directory refreash functionality.
		return true;
	}
	m_CoreReference->SystemLog("Failed to scan for project assets.");
	return false;
}
bool AssetManager::SetupEditorDirectories() {

	if (m_EditorRoot != nullptr) {
		m_CoreReference->SystemLog("Started scanning for editor assets to load...");
		ScanDirectory(*m_EditorRoot, true); //Sets up tree and Connections + Assets in each directory
		m_CoreReference->SystemLog("Finished scanning for editor assets to load.");
		return true;
	}
	m_CoreReference->SystemLog("Failed to scan for editor assets.");
	return false;
}

//Material serialization - Material file - Sprites are instead saved as names. If not found on loading then default is put instead
//Add Asset menu to content browser for adding assets like the material
//Need so storage for materials. MaterialsStorage?
//Maybe thats where shaders end up in the future. Toggle bool to enable feature and shader is decided on those enables bools



void AssetManager::ScanDirectory(Directory& parent, bool editorDirectory) {

	for (auto& directory : std::filesystem::directory_iterator(parent.m_Path)) {
		if (directory.is_regular_file()) {
			Asset* NewAsset = new Asset;
			NewAsset->m_Path = directory;
			parent.m_Assets.emplace_back(NewAsset);
			SetupAsset(*NewAsset, editorDirectory);
		}
		else {

			Directory* NewDirectory = new Directory(directory.path(), directory.path().filename().string());
			NewDirectory->m_ParentPath = parent.m_Path;
			parent.m_Folders.emplace_back(NewDirectory);

			if (editorDirectory)
				m_EditorDirectories.emplace_back(NewDirectory);
			else
				m_ProjectDirectories.emplace_back(NewDirectory); //To avoid having to go through them recursively for searching

			ScanDirectory(*NewDirectory, editorDirectory);
		}
	}
}
void AssetManager::SetupAsset(Asset& asset, bool editorAsset) {

	auto Extension = asset.m_Path.extension();
	auto Name = asset.m_Path.filename().replace_extension();
	asset.m_Extension = Extension.string();
	asset.m_Name = Name.string();

	//Any new supported asset types should be added here for checking
	if (Extension == ".jpg" || Extension == ".png") {
		asset.m_Type = AssetType::TEXTURE;

		if (editorAsset) {
			asset.m_EditorAsset = true;
			m_EditorTextureAssets.emplace_back(&asset);
		}
		else {
			asset.m_EditorAsset = false;
			m_ProjectTextureAssets.emplace_back(&asset);
		}
	}

	//TODO: Add materials type. Custom! .mt? .pr as general engine file with header?

}
bool AssetManager::LoadProjectAssets() {

	//IMPORTANT NOTE: Some images will be flipped when loaded. The problem is that i cant know which will be. So at some point i should implement the option to flip 
	//-them in engine. Maybe texture editor? Tick the option and the image will be reloaded!

	//IMPORTANT NOTE: String_view is meant to keep track of one block of memory. So assigning to it a copy will be invalid since it will keep track of the copies data
	
	
	m_CoreReference->SystemLog("Started loading project assets...");
	for (auto& TextureAsset : m_ProjectTextureAssets) {
		std::string FileName = TextureAsset->m_Path.filename().replace_extension().string();
		if (!m_TextureStorageReference->LoadTexture2D(*TextureAsset)) {
			m_CoreReference->SystemLog("Asset failed to load [Texture] [" + FileName + "] " + TextureAsset->m_Path.string());
		}
		else {
			m_CoreReference->SystemLog("Asset loaded successfully [Texture] [" + FileName + "] " + TextureAsset->m_Path.string());
		}
	}

	m_CoreReference->SystemLog("Finished loading project assets successfully!");
	return true;
}
bool AssetManager::LoadEditorAssets() {

	m_CoreReference->SystemLog("Started loading editor assets...");
	for (auto& TextureAsset : m_EditorTextureAssets) {
		if (!m_TextureStorageReference->LoadEditorTexture2D(*TextureAsset)) {
			m_CoreReference->SystemLog("Asset failed to load [Texture] [" + TextureAsset->m_Name + "] " + TextureAsset->m_Path.string());
		}
		else {
			m_CoreReference->SystemLog("Asset loaded successfully [Texture] [" + TextureAsset->m_Name + "] " + TextureAsset->m_Path.string());
		}
	}

	m_CoreReference->SystemLog("Finished loading editor assets successfully!");
	return true;
}


void AssetManager::CleanUpAssets() noexcept {

	//IMPORTANT NOTE: This needs to be done for every stored asset type cause they are copied into the new created assets CURRENTLY. So these are not even used!

	if (!m_EditorTextureAssets.empty()) {
		auto CleanUp = [](Asset* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_EditorTextureAssets), std::end(m_EditorTextureAssets), CleanUp);
		m_EditorTextureAssets.clear();
	}
	if (!m_ProjectTextureAssets.empty()) {
		auto CleanUp = [](Asset* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_ProjectTextureAssets), std::end(m_ProjectTextureAssets), CleanUp);
		m_ProjectTextureAssets.clear();
	}



}
void AssetManager::CleanUpDirectories() noexcept {

	if (!m_EditorDirectories.empty()) {
		auto CleanUp = [](Directory* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_EditorDirectories), std::end(m_EditorDirectories), CleanUp);
		m_EditorDirectories.clear();
	}
	if (!m_ProjectDirectories.empty()) {
		auto CleanUp = [](Directory* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_ProjectDirectories), std::end(m_ProjectDirectories), CleanUp);
		m_ProjectDirectories.clear();
	}
}