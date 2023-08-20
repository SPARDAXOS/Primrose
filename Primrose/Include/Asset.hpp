#pragma once
#include "Directory.hpp"


enum class AssetType {
	INVALID = 0,
	TEXTURE = 1,
	MATERIAL = 2,
	MODEL = 3,
	SCENE = 4
};
class Asset {
public:
	Asset() = default;
	~Asset() = default;

	Asset(const Asset& other) noexcept {
		*this = other;
	}
	Asset& operator=(const Asset& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Parent = other.m_Parent;
			this->m_Path = other.m_Path;
			this->m_Extension = other.m_Extension;
			this->m_Name = other.m_Name;
			this->m_EditorAsset = other.m_EditorAsset;
			this->m_Type = other.m_Type;
			return *this;
		}
	}

	Asset(Asset&& other) noexcept {
		*this = std::move(other);
	}
	Asset& operator=(Asset&& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Parent = std::move(other.m_Parent);
			this->m_Path = std::move(other.m_Path);
			this->m_Extension = std::move(other.m_Extension);
			this->m_Name = std::move(other.m_Name);
			this->m_EditorAsset = std::move(other.m_EditorAsset);
			this->m_Type = std::move(other.m_Type);
			return *this;
		}
	}

	bool operator==(const Asset& other) const noexcept {
		//NOTE: Might be enough to compare the path since it is not possible to have 2 assets with the same path.
		if (m_Path != other.m_Path)
			return false;
		else if (m_Parent != other.m_Parent)
			return false;
		else if (m_Extension != other.m_Extension)
			return false;
		else if (m_Name != other.m_Name)
			return false;
		else if (m_EditorAsset != other.m_EditorAsset)
			return false;
		else if (m_Type != other.m_Type)
			return false;
		else
			return true;
	}
	bool operator!=(const Asset& other) const noexcept {
		if (!(*this == other))
			return true;
		else
			return false;
	}


	Directory* m_Parent		{ nullptr };
	std::filesystem::path m_Path;
	std::string m_Extension;
	std::string m_Name;

	bool m_EditorAsset	{ false };
	AssetType m_Type	{ AssetType::INVALID };
};