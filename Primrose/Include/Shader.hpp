#pragma once
#include <GLAD/glad/glad.h>
#include "GLErrorHandling.hpp"
#include "Systems/AssetManager.hpp"

class Shader final {
public:
	Shader() = delete;
	Shader(GLenum type, const std::string_view& filePath);
	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&&) = delete;
	Shader& operator=(Shader&&) = delete;

public:
	inline GLuint GetID() const noexcept { return m_ID; };

private:
	[[nodiscard]] bool LoadShader(const std::string_view& filePath);
	[[nodiscard]] bool CompileShader() noexcept;

private:
	[[nodiscard]] bool CheckCompileStatus(const GLuint& ID) const noexcept;

private:
	GLuint m_ID;
	GLenum m_Type;
	std::string m_Source;
	bool m_IsValid;
};