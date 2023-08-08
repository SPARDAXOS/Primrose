#include "Directory.hpp"
#include "Asset.hpp"


bool Directory::DoesAssetExist(const std::string_view& path) const {
	for (uint32 index = 0; index < m_Assets.size(); index++) {
		if (m_Assets.at(index)->m_Path == path)
			return true;
	}
	return false;
}
bool Directory::DoesFolderExist(const std::string_view& path) const {
	for (uint32 index = 0; index < m_Folders.size(); index++) {
		if (m_Folders.at(index)->m_Path == path)
			return true;
	}
	return false;
}

bool Directory::RemoveAssetEntry(const Asset& asset) {

	for (uint32 index = 0; m_Assets.size(); index++) {
		if (*m_Assets.at(index) == asset) {
			m_Assets.erase(std::begin(m_Assets) + index);
			return true;
		}
	}
	return false;
}
bool Directory::RemoveFolderEntry(const Directory& directory) {

	for (uint32 index = 0; m_Folders.size(); index++) {
		if (*m_Folders.at(index) == directory) {
			m_Folders.erase(std::begin(m_Folders) + index);
			return true;
		}
	}
	return false;
}