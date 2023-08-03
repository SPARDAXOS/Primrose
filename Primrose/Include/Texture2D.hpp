#pragma once
#include "Systems/AssetManager.hpp" //Asset which maybe should be its own file
#include "GLAD/glad/glad.h"
#include "GLErrorHandling.hpp"

#include "STB_Image/STB_Image.h"



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


struct Texture2DSourceData final {
	uint8* m_Data{ nullptr };
	int32 m_Width{ 0 };
	int32 m_Height{ 0 };
	int32 m_ColorChannelCount{ 0 };
};
class Texture2D final {
public:

	Texture2D() = delete;
	Texture2D(const Asset& asset, Texture2DSourceData data) noexcept
		: m_Asset(&asset), m_Source(data)
	{
		GLCall(glGenTextures(1, &m_ID));
		if (m_ID != 0 && m_Asset != nullptr)
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
			PrintMessage("[User Error] Attempted to bind invalid Texture2D.");
			return;
		}
		GLCall(glBindTexture(GL_TEXTURE_2D, m_ID));

		int Channels = GL_RGB;
		if (GetColorChannelCount() == 3)
			Channels = GL_RGB;
		else if (GetColorChannelCount() == 4)
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
	[[nodiscard]] inline bool IsValid() const noexcept { return m_IsValid; }
	inline GLuint GetID() const noexcept { return m_ID; }
	inline int32 GetWidth() const noexcept { return m_Source.m_Width; }
	inline int32 GetHeight() const noexcept { return m_Source.m_Height; }
	inline int32 GetColorChannelCount() const noexcept { return m_Source.m_ColorChannelCount; }
	inline uint8* GetData() const noexcept { return m_Source.m_Data; }
	inline std::string_view GetName() const noexcept { return m_Asset->m_Name; }
	inline std::string_view GetExtension() const noexcept { return m_Asset->m_Extension; }
	inline std::string_view GetFilePath() const { return m_Asset->m_Path.string(); }

private:
	bool m_IsValid;

private:
	GLuint m_ID;
	const Asset* m_Asset;
	Texture2DSourceData m_Source;
};