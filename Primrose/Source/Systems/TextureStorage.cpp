#include "Systems/TextureStorage.hpp"
#include "Systems/Core.hpp"


TextureStorage::TextureStorage(Core& core) noexcept
	: m_CoreReference(&core)
{
}
TextureStorage::~TextureStorage() {
	if (!m_Texture2DStorage.empty()) {
		auto CleanUp = [](Texture2D* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_Texture2DStorage), std::end(m_Texture2DStorage), CleanUp);
		m_Texture2DStorage.clear();
	}
	if (!m_EditorTexture2DStorage.empty()) {
		auto CleanUp = [](Texture2D* ptr) {
			delete ptr;
		};
		std::for_each(std::begin(m_EditorTexture2DStorage), std::end(m_EditorTexture2DStorage), CleanUp);
		m_EditorTexture2DStorage.clear();
	}

	//m_CoreReference->SystemLog("[Exit] TextureStorage was exited successfully.");
}


bool TextureStorage::LoadTexture2D(Asset& asset, bool flipped) {

	if ((IsTexture2DLoaded(asset.m_Path.string()))) {
		return true;
	}
	else {
		Texture2DSourceData Buffer;
		stbi_set_flip_vertically_on_load(flipped);
		if (LoadFromFile(asset, Buffer)) {
			Texture2D* NewTexture2D = new Texture2D(asset, Buffer);
			m_Texture2DStorage.emplace_back(NewTexture2D);
			return true;
		}
		else
			return false;
	}
}
bool TextureStorage::LoadEditorTexture2D(Asset& asset, bool flipped) {

	if (IsTexture2DLoaded(asset.m_Path.string())) {
		return true;
	}
	else {
		Texture2DSourceData Buffer;
		stbi_set_flip_vertically_on_load(flipped);
		if (LoadFromFile(asset, Buffer)) {
			Texture2D* NewTexture2D = new Texture2D(asset, Buffer);
			m_EditorTexture2DStorage.emplace_back(NewTexture2D);
			return true;
		}
		else
			return false;
	}
}
bool TextureStorage::UnloadTexture2D(const std::string_view& path) {

	int32 TextureIndex = -1;
	if (!IsTexture2DLoaded(path, &TextureIndex)) {
		m_CoreReference->SystemLog(std::string("Failed to find texture to unload at " + *path.data()));
		return false;
	}
	else {
		delete m_Texture2DStorage.at(TextureIndex);
		m_Texture2DStorage.erase(std::begin(m_Texture2DStorage) + TextureIndex);
		return true;
	}
}

bool TextureStorage::GetTexture2DByName(const std::string_view& name, Texture2D*& ptr) const noexcept {

	for (auto& texture : m_Texture2DStorage) {
		if (texture->GetName() == name) {
			ptr = texture;
			return true;
		}
	}
	return false;
}
bool TextureStorage::GetTexture2DByPath(const std::string_view& path, Texture2D*& ptr) const noexcept {

	for (auto& texture : m_Texture2DStorage) {
		if (texture->GetFilePath() == path) {
			ptr = texture;
			return true;
		}
	}
	return false;
}

bool TextureStorage::GetEditorTexture2DByName(const std::string_view& name, Texture2D*& ptr) const noexcept {

	for (auto& texture : m_EditorTexture2DStorage) {
		if (texture->GetName() == name) {
			ptr = texture;
			return true;
		}
	}
	return false;
}
bool TextureStorage::GetEditorTexture2DByPath(const std::string_view& path, Texture2D*& ptr) const noexcept {

	for (auto& texture : m_EditorTexture2DStorage) {
		if (texture->GetFilePath() == path) {
			ptr = texture;
			return true;
		}
	}
	return false;
}


void TextureStorage::SetActiveTextureUnit(TextureUnit unit) noexcept {
	GLCall(glActiveTexture(static_cast<GLenum>(unit)));
}


bool TextureStorage::IsTexture2DLoaded(const std::string_view& path, int32* elementIndex) {

	for (uint32 index = 0; index < m_Texture2DStorage.size(); index++) {
		auto test = m_Texture2DStorage.at(index)->GetFilePath();
		if (test == path) {
			if (elementIndex != nullptr)
				*elementIndex = static_cast<int32>(index);
			return true;
		}
	}
	return false;
}
bool TextureStorage::LoadFromFile(const Asset& asset, Texture2DSourceData& buffer) {

	std::string AssetPath = asset.m_Path.string();
	std::string AssetName = asset.m_Name;
	unsigned char* data = stbi_load(AssetPath.data(), &buffer.m_Width, &buffer.m_Height, &buffer.m_ColorChannelCount, 0);
	if (data == nullptr)
		return false;
	else {
		buffer.m_Data = data;
		return true;
	}
}