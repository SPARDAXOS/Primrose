#pragma once
#include "AssetManager.hpp"
#include "Texture2D.hpp"
#include "Utility.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include "STB_Image/STB_Image.h"
#include "GLAD/glad/glad.h"
#include "GLErrorHandling.hpp"
#include <algorithm>






namespace TextureUnit {
	static constexpr int32 DIFFUSE = 0;
	static constexpr int32 AMBIENT = 1;
	static constexpr int32 SPECULAR = 2;

}

class TextureStorage final {
public:

	explicit TextureStorage() noexcept = default;
	~TextureStorage() {
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

		//TODO: Get Editor and log this message! Log file saved somewhere in the project?
		PrintMessage("[Exit] TextureStorage was exited successfully.");
	}

	TextureStorage(const TextureStorage&) = delete;
	TextureStorage& operator=(const TextureStorage&) = delete;

	TextureStorage(TextureStorage&&) = delete;
	TextureStorage& operator=(TextureStorage&&) = delete;

public:
	//NOTE: With the introduction of "Asset", this api is now weird to work with. Either provide some API for loading specific textures
	//- at specific directories and constructing a valid Asset out of it
	//- OR simply change it so that everything is loaded and you simple call GetTexture to get them
	//IMPORTANT: Why am i even calling it texture 2d? Just cause there are Texture 1D and 3D? I dont know if i will be adding support to those.



	[[nodiscard]] bool LoadTexture2D(Asset& asset, bool flipped = false) {

		if (IsTexture2DLoaded(asset.m_Path.string())) {
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
	[[nodiscard]] bool LoadEditorTexture2D(Asset& asset, bool flipped = false) {

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


	//Unfinished after rework - Should probably not be public API
	void UnloadTexture2D(const std::string_view path) {
		Texture2D* TargetTexture;
		if (!IsTexture2DLoaded(path)) //Make it return the index of where it found it!
			return;
		else {
			delete TargetTexture;
			//m_Storage.erase(std::begin()path);
		}
	}


	[[nodiscard]] bool GetTexture2DByName(const std::string_view name, Texture2D*& ptr) noexcept {

		for (auto& texture : m_Texture2DStorage) {
			if (texture->GetName() == name) {
				ptr = texture;
				return true;
			}
		}
		return false;
	}
	[[nodiscard]] bool GetTexture2DByPath(const std::string_view path, Texture2D*& ptr) {

		for (auto& texture : m_Texture2DStorage) {
			if (texture->GetFilePath() == path) {
				ptr = texture;
				return true;
			}
		}
		return false;
	}

	[[nodiscard]] bool GetEditorTexture2DByName(const std::string_view name, Texture2D*& ptr) noexcept {

		for (auto& texture : m_EditorTexture2DStorage) {
			if (texture->GetName() == name) {
				ptr = texture;
				return true;
			}
		}
		return false;
	}
	[[nodiscard]] bool GetEditorTexture2DByPath(const std::string_view path, Texture2D*& ptr) {

		for (auto& texture : m_EditorTexture2DStorage) {
			if (texture->GetFilePath() == path) {
				ptr = texture;
				return true;
			}
		}
		return false;
	}

	std::vector<Texture2D*> GetTexture2DStorage() const noexcept { return m_Texture2DStorage; }
	std::vector<Texture2D*> GetEditorTexture2DStorage() const noexcept { return m_EditorTexture2DStorage; }

public:
	void SetActiveTextureUnit(GLenum unit) noexcept {
		GLCall(glActiveTexture(unit));
	}

private:
	[[nodiscard]] bool IsTexture2DLoaded(const std::string_view path) { 

		for (auto& texture : m_Texture2DStorage) {
			if (texture->GetFilePath() == path)
				return true;
		}
		return false;
	}
	[[nodiscard]] bool LoadFromFile(const Asset& asset, Texture2DSourceData& buffer) {

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

private:
	std::vector<Texture2D*> m_Texture2DStorage;
	std::vector<Texture2D*> m_EditorTexture2DStorage;
};
