#pragma once
#include <filesystem>
#include <string_view>

#include "Utility.hpp"

class Asset;

class Directory final {
public:
	Directory() = delete;
	Directory(std::filesystem::path path, std::string_view name)
		: m_Path(path), m_Name(name)
	{
	}

	bool operator==(const Directory& other) const noexcept {
		//NOTE: Might be enough to compare the path since it is not possible to have 2 assets with the same path.
		if (m_Path != other.m_Path)
			return false;
		else if (m_ParentPath != other.m_ParentPath)
			return false;
		else if (m_Name != other.m_Name)
			return false;
		else if (m_Assets != other.m_Assets) 
			return false;
		else if (m_Folders != other.m_Folders)
			return false;
		else
			return true;
	}
	bool operator!=(const Directory& other) const noexcept {
		if (!(*this == other))
			return true;
		else
			return false;
	}



	bool DoesAssetExist(const std::string_view& path) const;
	bool DoesFolderExist(const std::string_view& path) const;

	bool RemoveAssetEntry(const Asset& asset);
	bool RemoveFolder(const Directory& directory);

	std::filesystem::path m_Path;
	std::filesystem::path m_ParentPath;
	std::string m_Name;
	std::vector<Asset*> m_Assets;
	std::vector<Directory*> m_Folders;
};