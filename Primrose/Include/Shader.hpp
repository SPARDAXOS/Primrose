#pragma once
#include <GLAD/glad/glad.h>
#include "GLErrorHandling.hpp"
#include "AssetManager.hpp"

class Shader final {
public:

	Shader() = delete;
	Shader(GLenum type, const std::string_view& filePath) {
		m_IsValid = true;
		m_Type = type;
		if (!LoadShader(filePath))
			m_IsValid = false;
		if (!CompileShader())
			m_IsValid = false;
	}
	~Shader() {
		GLCall(glDeleteShader(m_ID));
	}

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&&) = delete;
	Shader& operator=(Shader&&) = delete;

public:
	inline GLuint GetID() const noexcept { return m_ID; };

private:
	[[nodiscard]] bool LoadShader(const std::string_view& filePath) {
		return AssetManager::CRead(filePath, m_Source);
	}
	[[nodiscard]] bool CompileShader() noexcept {
		ClearGLErrors();
		m_ID = glCreateShader(m_Type);
		CheckGLError("CompileShader", __FILE__, __LINE__);

		const GLchar* PointerToSource = m_Source.data();
		GLCall(glShaderSource(m_ID, 1, &PointerToSource, nullptr));
		GLCall(glCompileShader(m_ID));

		return CheckCompileStatus(m_ID);
	}

private:
	[[nodiscard]] bool CheckCompileStatus(const GLuint& ID) const noexcept {
		int CompilationResults;
		GLCall(glGetShaderiv(m_ID, GL_COMPILE_STATUS, &CompilationResults));
		if (CompilationResults == GL_FALSE) {
			char ErrorMessage[512];
			GLCall(glGetShaderInfoLog(m_ID, 512, nullptr, ErrorMessage));
			PrintMessage(ErrorMessage);
			return false;
		}

		return true;
	}


private:
	GLuint m_ID;
	GLenum m_Type;
	std::string m_Source;

	bool m_IsValid;
};