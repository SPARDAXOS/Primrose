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

bool AssetManager::LoadProjectFiles() {


	//TODO: Implement proper error handling for this class
	if (!CheckForContentDirectory())
		return false;

	if (!ScanForContent())
		return false;

	if (!LoadAssets())
		return false;

	//Maybe clean up the assets list here after they all been loaded.
	//There is no need to have an extra indirection so let the loading systems copy the asset objects!
	


	return true;
}

bool AssetManager::CheckForContentDirectory() {

	if (std::filesystem::exists("Content")) {
		//TODO: Implement proper error handling for this class
		m_CoreReference->SystemLog("Content folder was located!");
		return true;
	}

	return false;
}
bool AssetManager::ScanForContent() {

	m_CoreReference->SystemLog("Started scanning for assets to load...");
	for (auto& DirectoryPath : std::filesystem::directory_iterator(std::filesystem::current_path())) {
		if (DirectoryPath.path().filename() == "Content") {
			Directory* NewDirectory = new Directory(DirectoryPath.path());
			m_Root = NewDirectory;
			m_Directories.emplace_back(NewDirectory);

			ScanDirectory(DirectoryPath, *NewDirectory);
		}
	}

	return true;
}
void AssetManager::ScanDirectory(std::filesystem::path path, Directory& parent) {

	for (auto& directory : std::filesystem::directory_iterator(path)) {
		if (directory.is_regular_file()) {
			//TODO: Change this into data that could be fed to the logger at editor or something
			//std::cout << "File at: " << directory.path() << std::endl;

			Asset* NewAsset = new Asset(directory);
			parent.m_Assets.emplace_back(NewAsset);
			CheckAssetType(*NewAsset);
		}
		else {
			//std::cout << "Folder at: " << directory.path() << std::endl;
			Directory* NewDirectory = new Directory(directory.path());
			NewDirectory->m_ParentPath = parent.m_Path; //OR just path?
			parent.m_Folders.emplace_back(NewDirectory);
			m_Directories.emplace_back(NewDirectory); //To avoid having to go through them recursively for searching

			ScanDirectory(directory, *NewDirectory);
		}
	}
}
void AssetManager::CheckAssetType(Asset& asset) {

	auto Extension = asset.m_Path.extension();

	//Any new supported asset types should be added here for checking
	if (Extension == ".jpg" || Extension == ".png") {
		asset.m_Type = AssetType::TEXTURE;
		m_TexturesAssets.emplace_back(&asset);
	}

}
bool AssetManager::LoadAssets() {

	//IMPORTANT NOTE: Some images will be flipped when loaded. The problem is that i cant know which will be. So at some point i should implement the option to flip 
	//-them in engine. Maybe texture editor? Tick the option and the image will be reloaded!

	//IMPORTANT NOTE: String_view is meant to keep track of one block of memory. So assigning to it a copy will be invalid since it will keep track of the copies data
	m_CoreReference->SystemLog("Started loading assets...");
	for (auto& TextureAsset : m_TexturesAssets) {
		std::string FileName = TextureAsset->m_Path.filename().replace_extension().string();
		if (!m_TextureStorageReference->LoadTexture2D(*TextureAsset)) {
			m_CoreReference->SystemLog("Asset failed to load [Texture] [" + FileName + "] " + TextureAsset->m_Path.string());
		}
		else {
			m_CoreReference->SystemLog("Asset loaded successfully [Texture] [" + FileName + "] " + TextureAsset->m_Path.string());
		}
	}

	m_CoreReference->SystemLog("Finished loading assets successfully!");
	return true;
}