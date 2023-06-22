#pragma once
#include <GLAD/glad/glad.h>
#include "OpenGLResources.hpp"


//TODO: Use bits instead since these are in NDC so -1.0 -> 1.0

struct Vertex {
	float X, Y, Z;
};

struct Triangle {
	Vertex m_Data[3] = {	0.5f, -0.5f, 0.0f,
						   -0.5f, -0.5f, 0.0f,
							0.0f,  0.5f, 0.0f	};

	GLuint m_Indices[3] = {	0, 1, 2	};

};
struct Square {
	Vertex m_Data[4] = {	0.5f, -0.5f, 0.0f,
						   -0.5f, -0.5f, 0.0f,
							0.5f,  0.5f, 0.0f,
						   -0.5f,  0.5f, 0.0f	};
	
	GLuint m_Indices[6] = {	0, 2, 3,
							1, 0, 3	};
};

class VBO final {
public:
	VBO() = delete;
	VBO(const void* data, const GLuint size) noexcept {
		GLCall(glGenBuffers(1, &m_ID));
		Bind();
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, &data, GL_STATIC_DRAW));
		Unbind();
	}
	~VBO() {
		GLCall(glDeleteBuffers(1, &m_ID));
	}

public:
	void Bind() const noexcept {
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
	}
	void Unbind() const noexcept {
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

private:
	GLuint m_ID;
};


class VAO final {
public:
	VAO() noexcept {
		GLCall(glGenVertexArrays(1, &m_ID));
	}
	~VAO() {
		GLCall(glDeleteBuffers(1, &m_ID));
	}

public:
	void Bind() const noexcept {
		GLCall(glBindVertexArray(m_ID));
	}
	void Unbind() const noexcept {
		GLCall(glBindVertexArray(0));
	}

private:
	GLuint m_ID;
};


class EBO final {
public:
	EBO() = delete;
	EBO(const GLuint* data, const GLuint size) noexcept { //Could make smaller ones with shorts for better perf
		GLCall(glGenBuffers(1, &m_ID));
		Bind();
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
		Unbind();
	}
	~EBO() {
		GLCall(glDeleteBuffers(1, &m_ID));
	}

public:
	void Bind() const noexcept {
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
	}
	void Unbind() const noexcept {
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

private:
	GLuint m_ID;
};