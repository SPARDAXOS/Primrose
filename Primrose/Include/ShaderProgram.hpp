#pragma once
#include "Shader.hpp"
#include "Math.hpp"
#include <unordered_map>


class ShaderProgram final {
public:
	explicit ShaderProgram() noexcept {
		ClearGLErrors();
		m_ID = glCreateProgram();
		CheckGLError("CreateShaderProgram", __FILE__, __LINE__);
		if (m_ID == 0)
			m_IsValid = false;
		else
			m_IsValid = true;
	}
	~ShaderProgram() {
		GLCall(glDeleteProgram(m_ID));
	}

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	ShaderProgram(ShaderProgram&&) = delete;
	ShaderProgram& operator=(ShaderProgram&&) = delete;

public:
	void Bind() const noexcept {
		GLCall(glUseProgram(m_ID));
	}
	void Unbind() const noexcept {
		GLCall(glUseProgram(0));
	}

public:
	void AttachShader(const Shader& shader) const noexcept {
		GLCall(glAttachShader(m_ID, shader.GetID()));
	}
	void DetachShader(const Shader& shader) const noexcept {
		GLCall(glDetachShader(m_ID, shader.GetID()));
	}

public:
	[[nodiscard]] bool LinkShaderProgram() const noexcept {
		GLCall(glLinkProgram(m_ID));
		return CheckProgramLinkStatus();
	}

public:
	inline const GLuint GetID() const noexcept { return m_ID; }; //If the template is there then this is not needed! better encapsulation

public:
	template<typename T>
	void SetUniform(const std::string_view& name, T value) {};

	template<>
	void SetUniform<Color>(const std::string_view& name, Color value) {

		const int UniformLocation = FindUniformLocation(name);
		if (UniformLocation != -1) {
			GLCall(glUniform4f(UniformLocation, value.m_R, value.m_G, value.m_B, value.m_A));
		}
		else
			PrintMessages("Could not find requested uniform in shader program - Uniform: ", name.data());
	}

	template<>
	void SetUniform<int>(const std::string_view& name, int value) {
		const int UniformLocation = FindUniformLocation(name);
		if (UniformLocation != -1) {
			GLCall(glUniform1i(UniformLocation, value));
		}
		else
			PrintMessages("Could not find requested uniform in shader program - Uniform: ", name.data());
	}

	template<>
	void SetUniform<Vector3f>(const std::string_view& name, Vector3f value) {
		const int UniformLocation = FindUniformLocation(name);
		if (UniformLocation != -1) {
			GLCall(glUniform3f(UniformLocation, value.m_X, value.m_Y, value.m_Z));
		}
		else
			PrintMessages("Could not find requested uniform in shader program - Uniform: ", name.data());
	}

	template<>
	void SetUniform<glm::mat4>(const std::string_view& name, glm::mat4 value) {
		const int UniformLocation = FindUniformLocation(name);
		if (UniformLocation != -1) {
			GLCall(glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, glm::value_ptr(value)));
		}
		else
			PrintMessages("Could not find requested uniform in shader program - Uniform: ", name.data());
	}

	template<>
	void SetUniform<glm::mat3>(const std::string_view& name, glm::mat3 value) {
		const int UniformLocation = FindUniformLocation(name);
		if (UniformLocation != -1) {
			GLCall(glUniformMatrix3fv(UniformLocation, 1, GL_FALSE, glm::value_ptr(value)));
		}
		else
			PrintMessages("Could not find requested uniform in shader program - Uniform: ", name.data());
	}




private:
	[[nodiscard]] int FindUniformLocation(const std::string_view& name) {
		if (m_CachedUniforms.find(name) != m_CachedUniforms.end()) {
			return m_CachedUniforms[name];
		}
		else {
			ClearGLErrors();
			const GLint Location = glGetUniformLocation(m_ID, name.data()); //If -1 then it didnt find it!
			CheckGLError("FindUniformLocation", __FILE__, __LINE__);
			m_CachedUniforms[name.data()] = Location;
			return Location;
		}
	}
	[[nodiscard]] bool CheckProgramLinkStatus() const noexcept {

		GLint ProgramLinkResults;
		GLCall(glGetProgramiv(m_ID, GL_VALIDATE_STATUS, &ProgramLinkResults));
		if (ProgramLinkResults == GL_FALSE) {
			char ErrorMessage[512];
			GLCall(glGetProgramInfoLog(m_ID, 512, nullptr, ErrorMessage));
			PrintMessage(ErrorMessage);
			return false;
		}

		GLCall(glValidateProgram(m_ID));
		return true;
	}

private:
	std::unordered_map<std::string_view, int> m_CachedUniforms;

private: //The is valid is sketch and questionable but maybe something to keep track of the state of the shader program would be good like how many attached shaders and such!
	GLuint m_ID;
	bool m_IsValid;
};