#include "Systems/AssetManager.hpp"
#include "Systems/Core.hpp"


AssetManager::AssetManager(Core& core) noexcept
	: m_CoreReference(&core)
{
	m_TextureStorageReference = m_CoreReference->GetTextureStorage();
	m_SerializerReference = m_CoreReference->GetSerializer();
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

	if (!SerializeAssets())
		return false;

	m_CoreReference->SystemLog("AssetManager finished loading assets successfully!");
	return true;
}



bool AssetManager::CreateAsset(AssetType type, Directory& location) {

	switch (type)
	{
	case AssetType::INVALID:
		return false;
	case AssetType::TEXTURE:
		return false;
	case AssetType::MATERIAL:
		return CreateMaterialAssetFile(location);
	default:
		return false;
	}
}

/// <summary>
/// Creates a new material asset and file at directory.
/// </summary>
/// <param name="location"> -Directory to create the file at.</param>
/// <returns></returns>
bool AssetManager::CreateMaterialAssetFile(Directory& location) {

	//This is meant to be used to create a new asset for the first time.

	//Creating asset
	Asset* NewAsset = new Asset; //Rememeber to clean this if the function fails
	NewAsset->m_EditorAsset = false;
	NewAsset->m_Extension = ".rose"; //Depending on AssetType? or just general engine asset
	NewAsset->m_Type = AssetType::MATERIAL;
	NewAsset->m_Name = "NewMaterial";
	NewAsset->m_Parent = &location;
	NewAsset->m_Path = location.m_Path.string() + std::string("\\" + NewAsset->m_Name + NewAsset->m_Extension);

	//Naming
	int CopyNameIncrement = 0;
	while (location.DoesAssetExist(NewAsset->m_Path.string())) {
		CopyNameIncrement++;
		NewAsset->m_Name = NewAsset->m_Name + std::to_string(CopyNameIncrement);
		//IMPORTANT NOTE: Using Windows slashes but the function does work regardless. 
		//-However, using double slashes for consistency in Asset.m_Path and Directory.m_Path 
		NewAsset->m_Path = location.m_Path.string() + std::string("\\" + NewAsset->m_Name + NewAsset->m_Extension);
	}

	//Create material using the asset
	Material* NewMaterial = new Material(*NewAsset);

	//Create file
	if (!m_SerializerReference->CreateFile(*NewMaterial)) {
		delete NewAsset;
		delete NewMaterial;
		return false;
	}

	//Add to materials list
	m_MaterialStorage.emplace_back(NewMaterial);

	//Add to assets list
	m_MaterialAssets.emplace_back(NewAsset);

	//Add to directory assets list
	location.m_Assets.emplace_back(NewAsset);

	m_CoreReference->SystemLog("Successfully created new [Material] asset [" + NewAsset->m_Name + "]"); //TODO: This is a user action message

	return true;
}



template<>
bool AssetManager::RemoveAssetEntry<Texture2D>(Asset& asset) {

	auto AssetPath = asset.m_Path;
	for (uint32 index = 0; index < m_ProjectTextureAssets.size(); index++) {
		if (asset == *m_ProjectTextureAssets.at(index)) {
			delete m_ProjectTextureAssets.at(index);
			m_ProjectTextureAssets.erase(std::begin(m_ProjectTextureAssets) + index);
			m_CoreReference->SystemLog("Successfully deleted asset [Texture2D] at " + AssetPath.string());
			return true;
		}
	}
	m_CoreReference->SystemLog("Failed to find asset [Texture2D] to delete at " + AssetPath.string());
	return false;
}
template<>
bool AssetManager::RemoveAssetEntry<Material>(Asset& asset) {

	auto AssetPath = asset.m_Path;
	for (uint32 index = 0; index < m_MaterialAssets.size(); index++) {
		if (asset == *m_MaterialAssets.at(index)) {
			delete m_MaterialAssets.at(index);
			m_MaterialAssets.erase(std::begin(m_MaterialAssets) + index);
			m_CoreReference->SystemLog("Successfully deleted asset [Material] at " + AssetPath.string());
			return true;
		}
	}
	m_CoreReference->SystemLog("Failed to find asset [Material] to delete at " + AssetPath.string());
	return false;
}
bool AssetManager::RemoveFolderEntry(Directory& folder) {

	auto FolderPath = folder.m_Path;
	for (uint32 index = 0; index < m_ProjectDirectories.size(); index++) {
		if (folder == *m_ProjectDirectories.at(index)) {
			auto FolderName = folder.m_Name;
			delete m_ProjectDirectories.at(index);
			m_ProjectDirectories.erase(std::begin(m_ProjectDirectories) + index);
			m_CoreReference->SystemLog("Successfully deleted folder [" + FolderName + "]" + "at" + FolderPath.string());
			return true;
		}
	}
	m_CoreReference->SystemLog("Failed to find folder [" + folder.m_Name + "]" "to delete at " + FolderPath.string());
	return false;
}


bool AssetManager::CreateNewFolder(Directory& location) {
	
	std::string FolderName = "NewFolder";
	std::string FolderPath(location.m_Path.string() + "\\" + FolderName);

	int CopyNameIncrement = 0;
	while (location.DoesFolderExist(FolderPath)) {
		CopyNameIncrement++;
		FolderName = FolderName + std::to_string(CopyNameIncrement);
		FolderPath = location.m_Path.string() + "\\" + FolderName;
	}

	if (!std::filesystem::create_directory(FolderPath))
		return false;

	Directory* NewDirectory = new Directory(FolderPath.data(), FolderName);
	m_ProjectDirectories.emplace_back(NewDirectory);
	location.m_Folders.emplace_back(NewDirectory);
	NewDirectory->m_Parent = &location;

	return true;
}
bool AssetManager::RemoveAsset(Asset& asset) {

	if (asset.m_Type == AssetType::INVALID) {
		m_CoreReference->SystemLog("Attempted to delete asset with invalid type [" + asset.m_Name + "]");
		return false;
	}

	//NAME IS NAME + EXTENSION SO USUALLY YOU CAN HAVE 2 FILES WITH THE SAME NAME AT THE SAME PLACE ITS JUST THAT THE EXTENSION IS DIFFERENT!
	//Might be already done? If the manager loads it then it must mean its a different asset! Looking up images will be a problem tho! cause it just checks name!
	//Fix find texture by name to also consider the extension then! add comment to function too!


	//Data -> Directory Entry -> Asset -> File
	switch (asset.m_Type) {
	case AssetType::TEXTURE: {

		if (!m_TextureStorageReference->UnloadTexture2D(asset.m_Path.string()))
			return false;

		if (!asset.m_Parent->RemoveAssetEntry(asset)) {
			m_CoreReference->SystemLog("Failed to remove asset entry from directory  " + asset.m_Parent->m_Path.string());
			return false;
		}

		auto AssetPath = asset.m_Path;
		if (!RemoveAssetEntry<Texture2D>(asset))
			return false;

		if (!m_SerializerReference->DeleteFile(AssetPath))
			return false;

		return true;
		
	}break;
	case AssetType::MATERIAL: {

		if (!RemoveMaterialFromStorage(asset))
			return false;

		if (!asset.m_Parent->RemoveAssetEntry(asset)) {
			m_CoreReference->SystemLog("Failed to remove asset entry from directory  " + asset.m_Parent->m_Path.string());
			return false;
		}

		auto AssetPath = asset.m_Path;
		if (!RemoveAssetEntry<Material>(asset))
			return false;

		if (!m_SerializerReference->DeleteFile(AssetPath))
			return false;

		return true;

	}break;
	}




	//Notes for later features!
	//When it comes to deleting and such. There is a bunch of problems that would need to be tackled such as
	//What if i delete an asset from windows while the engine is open? and edge cases like this.

	return false;
}
bool AssetManager::RemoveDirectory(Directory& directory) {

	//IMPORTANT NOTE: For every loop that deletes and releases all elements.
	//-free memory then deleting pointer then go for next element will access out of range since the range changed!
	//-I dont think this would happen in any of the lambdas cause i use begin and end, however, any loop with numbers will break for sure!

	while (directory.m_Folders.size() > 0)
		RemoveDirectory(*directory.m_Folders.at(0));

	while (directory.m_Assets.size() > 0)
		RemoveAsset(*directory.m_Assets.at(0));

	if (!directory.m_Parent->RemoveFolderEntry(directory)) {
		m_CoreReference->SystemLog("Failed to remove directory entry from parent entries."); //TODO: Doesnt need to be here if logger was in core
		return false;
	}
	auto FolderPath = directory.m_Path;
	if (!RemoveFolderEntry(directory))
		return false;

	if (!m_SerializerReference->DeleteFolder(FolderPath))
		return false;

	return true;
}


bool AssetManager::RemoveMaterialFromStorage(const Asset& asset) {

	for (uint32 index = 0; index < m_MaterialStorage.size(); index++) {
		if (asset == m_MaterialStorage.at(index)->GetAsset()) {
			delete m_MaterialStorage.at(index);
			m_MaterialStorage.erase(std::begin(m_MaterialStorage) + index);
			m_CoreReference->SystemLog("Successfully removed Material [" + asset.m_Name + "]" + " from materials storage.");
			return true;;
		}
	}
	m_CoreReference->SystemLog("Failed to remove Material [" + asset.m_Name + "]" + " from materials storage.");
	return false;
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


void AssetManager::ScanDirectory(Directory& parent, bool editorDirectory) {

	for (auto& directory : std::filesystem::directory_iterator(parent.m_Path)) {
		if (directory.is_regular_file()) {
			Asset* NewAsset = new Asset;
			NewAsset->m_Path = directory;
			NewAsset->m_Parent = &parent;
			parent.m_Assets.emplace_back(NewAsset);
			SetupAsset(*NewAsset, editorDirectory);
		}
		else {

			Directory* NewDirectory = new Directory(directory.path(), directory.path().filename().string());
			NewDirectory->m_Parent = &parent;
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
	asset.m_EditorAsset = editorAsset;

	//Any new supported asset types should be added here for checking
	if (Extension == ".jpg" || Extension == ".png") {
		asset.m_Type = AssetType::TEXTURE;

		if (editorAsset)
			m_EditorTextureAssets.emplace_back(&asset);
		else
			m_ProjectTextureAssets.emplace_back(&asset);
	}
	else if (Extension == ".rose") {
		asset.m_Type = m_SerializerReference->GetAssetTypeFromFile(asset);
		if (asset.m_Type == AssetType::MATERIAL)
			m_MaterialAssets.emplace_back(&asset);


	}
}
bool AssetManager::LoadProjectAssets() {

	
	m_CoreReference->SystemLog("Started loading project assets...");

	//////////
	//	Texture2D
	//////////
	for (auto& TextureAsset : m_ProjectTextureAssets) {
		if (!m_TextureStorageReference->LoadTexture2D(*TextureAsset)) {
			m_CoreReference->SystemLog("Asset failed to load [Texture] [" + TextureAsset->m_Name + "] " + TextureAsset->m_Path.string());
		}
		else {
			m_CoreReference->SystemLog("Asset loaded successfully [Texture] [" + TextureAsset->m_Name + "] " + TextureAsset->m_Path.string());
		}
	}


	//////////
	//	Materials
	//////////
	for (auto& MaterialAsset : m_MaterialAssets) {
		Material* NewMaterial = new Material(*MaterialAsset);
		m_MaterialStorage.emplace_back(NewMaterial);
		m_CoreReference->SystemLog("Added asset entry successfully [Material] [" + MaterialAsset->m_Name + "] " + MaterialAsset->m_Path.string());
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
bool AssetManager::SerializeAssets() {

	m_CoreReference->SystemLog("Started serializing project assets...");

	//////////
	//	Materials
	//////////
	for (uint32 index = 0; index < m_MaterialStorage.size(); index++) {
		Material* MaterialAsset = m_MaterialStorage.at(index);
		if (MaterialAsset == nullptr) {
			m_CoreReference->SystemLog("Invalid material ptr was found in material storage while serializing assets.");
			m_MaterialStorage.erase(std::begin(m_MaterialStorage) + index);
			continue;
		}
		else if (!m_SerializerReference->SerializeFromFile<Material>(*MaterialAsset)) {
			m_CoreReference->SystemLog("Error serializing asset [" + MaterialAsset->GetAsset().m_Name + "] from file.");
			delete MaterialAsset;
			m_MaterialStorage.erase(std::begin(m_MaterialStorage) + index);
		}
		m_CoreReference->SystemLog("Asset serialized successfully [Material] [" + MaterialAsset->GetAsset().m_Name + "] " + MaterialAsset->GetAsset().m_Path.string());
	}


	m_CoreReference->SystemLog("Finished serializing project assets successfully!");
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
	if (!m_MaterialAssets.empty()) {
		auto CleanUp = [](Asset* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_MaterialAssets), std::end(m_MaterialAssets), CleanUp);
		m_MaterialAssets.clear();
	}


	//Maybe somewhere else?
	if (!m_MaterialStorage.empty()) {
		auto CleanUp = [](Material* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_MaterialStorage), std::end(m_MaterialStorage), CleanUp);
		m_MaterialStorage.clear();
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