#include "Shader.hpp"
#include "Systems/Serializer.hpp"




Shader::Shader(GLenum type, const std::string_view& filePath) {
	m_IsValid = true;
	m_Type = type;
	if (!LoadShader(filePath))
		m_IsValid = false;
	if (!CompileShader())
		m_IsValid = false;
}
Shader::~Shader() {
	GLCall(glDeleteShader(m_ID));
}

bool Shader::LoadShader(const std::string_view& filePath) {
	return Serializer::CRead(filePath, m_Source);
}
bool Shader::CompileShader() noexcept {
	ClearGLErrors();
	m_ID = glCreateShader(m_Type);
	CheckGLError("CompileShader", __FILE__, __LINE__);

	const GLchar* PointerToSource = m_Source.data();
	GLCall(glShaderSource(m_ID, 1, &PointerToSource, nullptr));
	GLCall(glCompileShader(m_ID));

	return CheckCompileStatus(m_ID);
}


bool Shader::CheckCompileStatus(const GLuint& ID) const noexcept {
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