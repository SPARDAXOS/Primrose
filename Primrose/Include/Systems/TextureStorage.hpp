#pragma once
#pragma warning(push, 0)
#include "STB_Image/stb_image.h"
#pragma warning(pop)



#include "AssetManager.hpp"
#include "Texture2D.hpp"
#include "Utility.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include "GLAD/glad/glad.h"
#include "GLErrorHandling.hpp"
#include <algorithm>





namespace TextureType {
	static constexpr int32 DIFFUSE = 0;
	static constexpr int32 AMBIENT = 1;
	static constexpr int32 SPECULAR = 2;

}
enum class TextureUnit {
	TEXTURE0 = GL_TEXTURE0,
	TEXTURE1 = GL_TEXTURE1,
	TEXTURE2 = GL_TEXTURE2,
	TEXTURE3 = GL_TEXTURE3,
	TEXTURE4 = GL_TEXTURE4,
	TEXTURE5 = GL_TEXTURE5,
	TEXTURE6 = GL_TEXTURE6,
	TEXTURE7 = GL_TEXTURE7,
	TEXTURE8 = GL_TEXTURE8,
	TEXTURE9 = GL_TEXTURE9,
	TEXTURE10 = GL_TEXTURE10,
	TEXTURE11 = GL_TEXTURE11,
	TEXTURE12 = GL_TEXTURE12,
	TEXTURE13 = GL_TEXTURE13,
	TEXTURE14 = GL_TEXTURE14,
	TEXTURE15 = GL_TEXTURE15,
};

class Core;

class TextureStorage final {
public:

	TextureStorage() noexcept = delete;
	TextureStorage(Core& core) noexcept;
	~TextureStorage();

	TextureStorage(const TextureStorage&) = delete;
	TextureStorage& operator=(const TextureStorage&) = delete;

	TextureStorage(TextureStorage&&) = delete;
	TextureStorage& operator=(TextureStorage&&) = delete;

public:
	[[nodiscard]] bool LoadTexture2D(Asset& asset, bool flipped = false);
	[[nodiscard]] bool LoadEditorTexture2D(Asset& asset, bool flipped = false);
	[[nodiscard]] bool UnloadTexture2D(const std::string_view& path);

	[[nodiscard]] bool GetTexture2DByName(const std::string_view& name, Texture2D*& ptr) const noexcept;
	[[nodiscard]] bool GetTexture2DByPath(const std::string_view& path, Texture2D*& ptr) const noexcept;

	[[nodiscard]] bool GetEditorTexture2DByName(const std::string_view& name, Texture2D*& ptr) const noexcept;
	[[nodiscard]] bool GetEditorTexture2DByPath(const std::string_view& path, Texture2D*& ptr) const noexcept;

	inline std::vector<Texture2D*> GetTexture2DStorage() const noexcept { return m_Texture2DStorage; }
	inline std::vector<Texture2D*> GetEditorTexture2DStorage() const noexcept { return m_EditorTexture2DStorage; }

	void SetActiveTextureUnit(TextureUnit unit) noexcept;

private:
	[[nodiscard]] bool IsTexture2DLoaded(const std::string_view& path, int32* elementIndex = nullptr);
	[[nodiscard]] bool LoadFromFile(const Asset& asset, Texture2DSourceData& buffer);

private:
	std::vector<Texture2D*> m_Texture2DStorage;
	std::vector<Texture2D*> m_EditorTexture2DStorage;

	Core* m_CoreReference	{ nullptr };
};
