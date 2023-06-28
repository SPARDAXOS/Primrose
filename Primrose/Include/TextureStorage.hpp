#pragma once
#include "FileManagment.hpp"
#include "Utility.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include "STB_Image/STB_Image.h"
#include "GLAD/glad/glad.h"
#include "GLErrorHandling.hpp"
#include <algorithm>



namespace TextureUnit {
	static constexpr uint8 DIFFUSE  = 0;
	static constexpr uint8 AMBIENT  = 1;
	static constexpr uint8 SPECULAR  = 2;
	
}


class Texture2DSourceData final {
public:
	uint8* m_Data;
	int32 m_Width;
	int32 m_Height;
	int32 m_ColorChannelCount;
	std::string_view m_Name;
};
class Texture2D final {
public:

	Texture2D() = delete;
	Texture2D(Texture2DSourceData& data) noexcept {
		m_Source = data;
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

	//Texture2D(const Texture2D& rhs) noexcept {
	//	this->m_IsValid = rhs.IsValid();
	//	this->m_ID = rhs.GetID();
	//	this->m_Source = rhs.m_Source;
	//}
	//Texture2D& operator=(const Texture2D& rhs) noexcept {
	//	this->m_IsValid = rhs.IsValid();
	//	this->m_ID = rhs.GetID();
	//	this->m_Source = rhs.m_Source;
	//	return *this;
	//}

	Texture2D(Texture2D&&) = delete;
	Texture2D& operator=(Texture2D&&) = delete;

	//Texture2D(Texture2D&& rhs) {
	//	this->m_IsValid = rhs.IsValid();
	//	this->m_ID = rhs.GetID();
	//	this->m_Source = rhs.m_Source;
	//}
	//Texture2D& operator=(Texture2D&& rhs) {
	//	this->m_IsValid = rhs.IsValid();
	//	this->m_ID = rhs.GetID();
	//	this->m_Source = rhs.m_Source;
	//	return *this;
	//}

	//bool operator==(const Texture2D& rhs) const noexcept {
	//	if (this->GetWidth() != rhs.GetWidth())
	//		return false;
	//	else if (this->GetHeight() != rhs.GetHeight())
	//		return false;
	//	else if (this->GetColorChannelCount() != rhs.GetColorChannelCount())
	//		return false;
	//	else if (this->GetID() != rhs.GetID())
	//		return false;
	//	else
	//		return true;
	//}
	//size_t operator()(const Texture2D& t) const
	//{
	//	return t.GetID();
	//}

public:
	void Bind() const noexcept {
		if (!m_IsValid) {
			PrintMessage("[User Error] Attempted to unbind invalid Texture2D.");
			return;
		}
		GLCall(glBindTexture(GL_TEXTURE_2D, m_ID));
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

private:
	bool m_IsValid;

private:
	GLuint m_ID;
	Texture2DSourceData m_Source;
};

class TextureStorage final {
public:

	explicit TextureStorage() noexcept = default;
	~TextureStorage() {
		if (!m_Storage.empty()) {
			auto Lambda = [](std::pair<std::string_view, Texture2D*> pair) { delete pair.second; };
			std::for_each(std::begin(m_Storage), std::end(m_Storage), Lambda);
			m_Storage.clear();
		}
		PrintMessage("[Exit] TextureStorage was exited successfully.");
	}

	TextureStorage(const TextureStorage&) = delete;
	TextureStorage& operator=(const TextureStorage&) = delete;

	TextureStorage(TextureStorage&&) = delete;
	TextureStorage& operator=(TextureStorage&&) = delete;

public:
	[[nodiscard]] bool LoadTexture2D(const std::string_view& path, const std::string_view& name, Texture2D*& ptr, bool flipped = true) {

		if (FindTexture2D(name, ptr)) {
			return true;
		}
		else {
			Texture2DSourceData Buffer;
			stbi_set_flip_vertically_on_load(flipped);
			if (LoadFromFile(path, name, Buffer)) {
				Texture2D* NewTexture2D = new Texture2D(Buffer);
				m_Storage.emplace(name, NewTexture2D);
				ptr = NewTexture2D;
				return true;
			}
			else
				return false;
		}
	}
	void UnloadTexture2D(const std::string_view& name) {
		Texture2D* TargetTexture;
		if (!FindTexture2D(name, TargetTexture))
			return;
		else {
			delete TargetTexture;
			m_Storage.erase(name);
		}
	}
	[[nodiscard]] bool GetTexture2D(const std::string_view& name, Texture2D*& ptr) {
		if (!FindTexture2D(name, ptr))
			return false;
		else
			return true;
	}

public:
	void ActivateTextureUnit(GLenum unit) noexcept {
		GLCall(glActiveTexture(unit));
	}
	void SetSamplerState(GLenum texture, GLenum pname, GLenum pvalue) noexcept {
		GLCall(glTexParameteri(texture, pname, pvalue));
	}

private:
	[[nodiscard]] bool FindTexture2D(const std::string_view& name, Texture2D*& ptr) {
		if (m_Storage.find(name) != m_Storage.end()) {
			ptr = m_Storage[name];
			return true;
		}
		else
			return false;
	}
	[[nodiscard]] bool LoadFromFile(const std::string_view& path, const std::string_view& name, Texture2DSourceData& buffer) noexcept {
		unsigned char* data = stbi_load(path.data(), &buffer.m_Width, &buffer.m_Height, &buffer.m_ColorChannelCount, 0);
		if (data == nullptr)
			return false;
		else {
			buffer.m_Data = data;
			buffer.m_Name = name;
			return true;
		}
	}


private:
	std::unordered_map<std::string_view, Texture2D*> m_Storage;

};
