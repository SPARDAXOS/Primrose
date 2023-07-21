#pragma once
#include "FileSystem.hpp"
#include "Utility.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include "STB_Image/STB_Image.h"
#include "GLAD/glad/glad.h"
#include "GLErrorHandling.hpp"
#include <algorithm>


//TODO: Should these even be here? Ye this is starting to pile up here. It all needs to be moved and the includes updated in the spriterenderer and the editor
enum class AddressingMode {
	MIRRORED = GL_MIRRORED_REPEAT,
	CLAMPED_TO_EDGE = GL_CLAMP_TO_EDGE,
	CLAMPED_TO_BORDER = GL_CLAMP_TO_BORDER,
	REPEAT = GL_REPEAT
};
enum class FilteringModeMin {
	LINEAR = GL_LINEAR,
	NEAREST = GL_NEAREST,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
	NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR
};
enum class FilteringModeMag {
	LINEAR = GL_LINEAR,
	NEAREST = GL_NEAREST
};
enum class BlendEquation {
	ADDITIVE = GL_FUNC_ADD,
	SUBTRACTIVE = GL_FUNC_SUBTRACT,
	REVERSE_SUBTRACTIVE = GL_FUNC_REVERSE_SUBTRACT
};
enum class SourceBlendMode {
	ZERO = GL_ZERO,
	ONE = GL_ONE,
	SOURCE_COLOR = GL_SRC_COLOR,
	ONE_MINUS_SOURCE_COLOR = GL_ONE_MINUS_SRC_COLOR,
	DESTINATION_COLOR = GL_DST_COLOR,
	ONE_MINUS_DESTINATION_COLOR = GL_ONE_MINUS_DST_COLOR,
	SOURCE_ALPHA = GL_SRC_ALPHA,
	ONE_MINUS_SOURCE_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
	DESTINATION_ALPHA = GL_DST_ALPHA,
	ONE_MINUS_DESTINATION_ALPHA = GL_ONE_MINUS_DST_ALPHA,
	CONSTANT_COLOR = GL_CONSTANT_COLOR,
	ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
	CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
	ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
	SOURCE_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE
};
enum class DestinationBlendMode {
	ZERO = GL_ZERO,
	ONE = GL_ONE,
	SOURCE_COLOR = GL_SRC_COLOR,
	ONE_MINUS_SOURCE_COLOR = GL_ONE_MINUS_SRC_COLOR,
	DESTINATION_COLOR = GL_DST_COLOR,
	ONE_MINUS_DESTINATION_COLOR = GL_ONE_MINUS_DST_COLOR,
	SOURCE_ALPHA = GL_SRC_ALPHA,
	ONE_MINUS_SOURCE_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
	DESTINATION_ALPHA = GL_DST_ALPHA,
	ONE_MINUS_DESTINATION_ALPHA = GL_ONE_MINUS_DST_ALPHA,
	CONSTANT_COLOR = GL_CONSTANT_COLOR,
	ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
	CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
	ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
};

namespace EnumToText {

	inline const char* ToText(AddressingMode type) {

		switch (type) {
		case AddressingMode::CLAMPED_TO_BORDER: {
			return "Clamped To Border";
		}
		case AddressingMode::CLAMPED_TO_EDGE: {
			return "Clamped To Edge";
		}
		case AddressingMode::MIRRORED: {
			return "Mirrored";
		}
		case AddressingMode::REPEAT: {
			return "Repeat";
		}
		default: {
			throw std::invalid_argument("Wrong argument type sent to EnumToText - AddressingMode");
		}
		}
	}
	inline const char* ToText(FilteringModeMin type) {

		switch (type) {
		case FilteringModeMin::LINEAR: {
			return "Linear";
		}
		case FilteringModeMin::LINEAR_MIPMAP_LINEAR: {
			return "Linear Mipmap Linear";
		}
		case FilteringModeMin::LINEAR_MIPMAP_NEAREST: {
			return "Linear Mipmap Nearest";
		}
		case FilteringModeMin::NEAREST: {
			return "Nearest";
		}
		case FilteringModeMin::NEAREST_MIPMAP_LINEAR: {
			return "Nearet Mipmap Linear";
		}
		case FilteringModeMin::NEAREST_MIPMAP_NEAREST: {
			return "Nearest Mipmap Nearest";
		}
		default: {
			throw std::invalid_argument("Wrong argument type sent to EnumToText - FilteringModeMin");
		}
		}
	}
	inline const char* ToText(FilteringModeMag type) {

		switch (type) {
		case FilteringModeMag::LINEAR: {
			return "Linear";
		}
		case FilteringModeMag::NEAREST: {
			return "Nearest";
		}
		default: {
			throw std::invalid_argument("Wrong argument type sent to EnumToText - FilteringModeMag");
		}
		}
	}
	inline const char* ToText(BlendEquation type) {

		switch (type) {
		case BlendEquation::ADDITIVE: {
			return "Additive";
		}
		case BlendEquation::SUBTRACTIVE: {
			return "Subtractive";
		}
		case BlendEquation::REVERSE_SUBTRACTIVE: {
			return "Reverse Subtractive";
		}
		default: {
			throw std::invalid_argument("Wrong argument type sent to EnumToText - BlendEquation");
		}
		}
	}
	inline const char* ToText(SourceBlendMode type) {

		switch (type) {
		case SourceBlendMode::ZERO: {
			return "Zero";
		}
		case SourceBlendMode::ONE: {
			return "One";
		}
		case SourceBlendMode::SOURCE_COLOR: {
			return "Source Color";
		}
		case SourceBlendMode::ONE_MINUS_SOURCE_COLOR: {
			return "One Minus Source Color";
		}
		case SourceBlendMode::DESTINATION_COLOR: {
			return "Destination Color";
		}
		case SourceBlendMode::ONE_MINUS_DESTINATION_COLOR: {
			return "One Minus Destination Color";
		}
		case SourceBlendMode::SOURCE_ALPHA: {
			return "Source Alpha";
		}
		case SourceBlendMode::ONE_MINUS_SOURCE_ALPHA: {
			return "One Minus Source Alpha";
		}
		case SourceBlendMode::DESTINATION_ALPHA: {
			return "Destination Alpha";
		}
		case SourceBlendMode::ONE_MINUS_DESTINATION_ALPHA: {
			return "One Minus Destination Alpha";
		}
		case SourceBlendMode::CONSTANT_COLOR: {
			return "Constant Color";
		}
		case SourceBlendMode::ONE_MINUS_CONSTANT_COLOR: {
			return "One Minus Constant Color";
		}
		case SourceBlendMode::CONSTANT_ALPHA: {
			return "Constant Alpha";
		}
		case SourceBlendMode::ONE_MINUS_CONSTANT_ALPHA: {
			return "One Minus Constant Alpha";
		}
		case SourceBlendMode::SOURCE_ALPHA_SATURATE: {
			return "Source Alpha Saturate";
		}
		default: {
			throw std::invalid_argument("Wrong argument type sent to EnumToText - SourceBlendMode");
		}
		}
	}
	inline const char* ToText(DestinationBlendMode type) {

		switch (type) {
		case DestinationBlendMode::ZERO: {
			return "Zero";
		}
		case DestinationBlendMode::ONE: {
			return "One";
		}
		case DestinationBlendMode::SOURCE_COLOR: {
			return "Source Color";
		}
		case DestinationBlendMode::ONE_MINUS_SOURCE_COLOR: {
			return "One Minus Source Color";
		}
		case DestinationBlendMode::DESTINATION_COLOR: {
			return "Destination Color";
		}
		case DestinationBlendMode::ONE_MINUS_DESTINATION_COLOR: {
			return "One Minus Destination Color";
		}
		case DestinationBlendMode::SOURCE_ALPHA: {
			return "Source Alpha";
		}
		case DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA: {
			return "One Minus Source Alpha";
		}
		case DestinationBlendMode::DESTINATION_ALPHA: {
			return "Destination Alpha";
		}
		case DestinationBlendMode::ONE_MINUS_DESTINATION_ALPHA: {
			return "One Minus Destination Alpha";
		}
		case DestinationBlendMode::CONSTANT_COLOR: {
			return "Constant Color";
		}
		case DestinationBlendMode::ONE_MINUS_CONSTANT_COLOR: {
			return "One Minus Constant Color";
		}
		case DestinationBlendMode::CONSTANT_ALPHA: {
			return "Constant Alpha";
		}
		case DestinationBlendMode::ONE_MINUS_CONSTANT_ALPHA: {
			return "One Minus Constant Alpha";
		}
		default: {
			throw std::invalid_argument("Wrong argument type sent to EnumToText - DestinationBlendMode");
		}
		}
	}
}






namespace TextureUnit {
	static constexpr uint8 DIFFUSE  = 0;
	static constexpr uint8 AMBIENT  = 1;
	static constexpr uint8 SPECULAR  = 2;
	
}


struct Texture2DSourceData final {
	uint8* m_Data				{ nullptr };
	int32 m_Width				{ 0 };
	int32 m_Height				{ 0 };
	int32 m_ColorChannelCount	{ 0 };
	std::string m_Name;
}; 
class Texture2D final : Asset {
public:

	Texture2D() = delete;
	Texture2D(Asset asset, Texture2DSourceData data) noexcept 
	{
		m_Source = data;
		m_Path = asset.m_Path;
		m_Type = asset.m_Type;

		GLCall(glGenTextures(1, &m_ID));
		if (m_ID != 0)
			m_IsValid = true;
		else
			m_IsValid = false;
	}
	~Texture2D() {
		if (m_IsValid) {
			GLCall(glDeleteTextures(1, &m_ID));
			stbi_image_free(m_Source.m_Data);
		}
	}

	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;

	Texture2D(Texture2D&&) = delete;
	Texture2D& operator=(Texture2D&&) = delete;

public:
	void Bind() const noexcept {
		if (!m_IsValid) {
			PrintMessage("[User Error] Attempted to unbind invalid Texture2D.");
			return;
		}
		GLCall(glBindTexture(GL_TEXTURE_2D, m_ID));

		int Channels = 0;
		if (GetColorChannelCount() == 3)
			Channels = GL_RGB;
		if (GetColorChannelCount() == 4)
			Channels = GL_RGBA;

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, Channels, m_Source.m_Width, m_Source.m_Height, 0, Channels, GL_UNSIGNED_BYTE, m_Source.m_Data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	void Unbind() const noexcept {
		if (!m_IsValid) {
			PrintMessage("[User Error] Attempted to unbind invalid Texture2D.");
			return;
		}
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

public:
	[[nodiscard]] inline bool IsValid() const noexcept { return m_IsValid; };
	inline GLuint GetID() const noexcept { return m_ID; };
	inline int32 GetWidth() const noexcept { return m_Source.m_Width; };
	inline int32 GetHeight() const noexcept { return m_Source.m_Height; };
	inline int32 GetColorChannelCount() const noexcept { return m_Source.m_ColorChannelCount; };
	inline uint8* GetData() const noexcept { return m_Source.m_Data; };
	inline std::string_view GetName() const noexcept { return m_Source.m_Name; }
	inline std::string_view GetFilePath() const { return m_Path.string(); }

private:
	bool m_IsValid;

private:
	GLuint m_ID;
	Texture2DSourceData m_Source;
};







class TextureStorage final {
public:

	explicit TextureStorage() noexcept = default;

	//NEEDS REWORK!
	~TextureStorage() {
		//if (!m_Storage.empty()) {
		//	auto Lambda = [](std::pair<std::string_view, Texture2D*> pair) { delete pair.second; };
		//	std::for_each(std::begin(m_Storage), std::end(m_Storage), Lambda);
		//	m_Storage.clear();
		//}
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


	//Old _____[[nodiscard]] bool LoadTexture2D(const std::string_view& path, const std::string_view& name, Texture2D*& ptr, bool flipped = true)

	[[nodiscard]] bool LoadTexture2D(Asset& asset, bool flipped = false) { //Flipped will be kinda problamatic now...

		std::string AssetPath = asset.m_Path.string();
		if (FindTexture2D(AssetPath)) {
			return true; // Should it just be true? If it was loaded already?
		}
		else {
			Texture2DSourceData Buffer;
			stbi_set_flip_vertically_on_load(flipped);
			if (LoadFromFile(asset, Buffer)) {
				Texture2D* NewTexture2D = new Texture2D(asset, Buffer);
				m_Storage.emplace_back(NewTexture2D);
				return true;
			}
			else
				return false;
		}
	}

	//Unfinished after rework
	void UnloadTexture2D(const std::string_view path) {
		Texture2D* TargetTexture;
		if (!FindTexture2D(path)) //Make it return the index of where it found it!
			return;
		else {
			delete TargetTexture;
			//m_Storage.erase(std::begin()path);
		}
	}

	//TODO: Add more finding functions. By Path?
	[[nodiscard]] bool GetTexture2D(const std::string_view name, Texture2D*& ptr) {

		for (auto& texture : m_Storage) {
			if (texture->GetName() == name) {
				ptr = texture;
				return true;
			}
		}
		return false;



		//for (auto& texture : m_Storage) {
		//	if (texture.second->GetName() == name) {
		//		ptr = texture.second;
		//		return true;
		//	}
		//}
	}

public:
	void ActivateTextureUnit(GLenum unit) noexcept {
		GLCall(glActiveTexture(unit));
	}

private:
	//Terrible function name - misleading
	[[nodiscard]] bool FindTexture2D(const std::string_view path) { 

		for (auto& texture : m_Storage) {
			if (texture->GetFilePath() == path)
				return true;
		}
		return false;
		//if (m_Storage.find(path) != m_Storage.end()) {
		//	//ptr = m_Storage[name];
		//	return true;
		//}
		//else
		//	return false;
	}
	[[nodiscard]] bool LoadFromFile(Asset& asset, Texture2DSourceData& buffer) {

		std::string AssetPath = asset.m_Path.string();
		std::string AssetName = asset.m_Path.filename().replace_extension().string();
		unsigned char* data = stbi_load(AssetPath.data(), &buffer.m_Width, &buffer.m_Height, &buffer.m_ColorChannelCount, 0);
		if (data == nullptr)
			return false;
		else {
			buffer.m_Data = data;
			buffer.m_Name = AssetName;
			return true;
		}
	}


private:
	//std::unordered_map<std::string_view, Texture2D*> m_Storage;
	std::vector<Texture2D*> m_Storage;
};
