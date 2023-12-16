#pragma once
#include <GLAD/glad/glad.h>
#include "GLErrorHandling.hpp"
#include "ShaderProgram.hpp"


//TODO: Use bits instead since these are in NDC so -1.0 -> 1.0

struct Vertex {
	Vector3f m_Position;
	Vector2f m_TextureCoordinates;
	Vector3f m_Normal;
};

struct Triangle {
	Vertex m_Data[3] = {	0.5f, -0.5f, 0.0f,
						   -0.5f, -0.5f, 0.0f,
							0.0f,  0.5f, 0.0f	};

	GLuint m_Indices[3] = {	0, 1, 2	};

};

//A lot of these are made with OpenGL coordinates system in mind!
struct Square {
	Vertex m_Data[4] = {	Vector3f( 0.5f, -0.5f, 0.0f), Vector2f(1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f(-0.5f, -0.5f, 0.0f), Vector2f(0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f( 0.5f,  0.5f, 0.0f), Vector2f(1.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f(-0.5f,  0.5f, 0.0f), Vector2f(0.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f)    };
	
	GLuint m_Indices[6] = {	0, 2, 3,
							1, 0, 3	};
};
struct Cube {
	Vertex m_Data[36] = {   //Back
							Vector3f(-0.5f, -0.5f, -0.5f), Vector2f(0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f( 0.5f, -0.5f, -0.5f), Vector2f(1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f( 0.5f,  0.5f, -0.5f), Vector2f(1.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f( 0.5f,  0.5f, -0.5f), Vector2f(1.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f(-0.5f,  0.5f, -0.5f), Vector2f(0.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f),
							Vector3f(-0.5f, -0.5f, -0.5f), Vector2f(0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f),

							//Front
							Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 0.0f),  Vector3f(0.0f, 0.0f, 1.0f),
							Vector3f( 0.5f, -0.5f, 0.5f), Vector2f(1.0f, 0.0f),	 Vector3f(0.0f, 0.0f, 1.0f),
							Vector3f( 0.5f,  0.5f, 0.5f), Vector2f(1.0f, 1.0f),	 Vector3f(0.0f, 0.0f, 1.0f),
							Vector3f( 0.5f,  0.5f, 0.5f), Vector2f(1.0f, 1.0f),	 Vector3f(0.0f, 0.0f, 1.0f),
							Vector3f(-0.5f,  0.5f, 0.5f), Vector2f(0.0f, 1.0f),	 Vector3f(0.0f, 0.0f, 1.0f),
							Vector3f(-0.5f, -0.5f, 0.5f), Vector2f(0.0f, 0.0f),	 Vector3f(0.0f, 0.0f, 1.0f),
	
							
							Vector3f(-0.5f,  0.5f,  0.5f), Vector2f(1.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f),
							Vector3f(-0.5f,  0.5f, -0.5f), Vector2f(1.0f, 1.0f), Vector3f(-1.0f, 0.0f, 0.0f),
							Vector3f(-0.5f, -0.5f, -0.5f), Vector2f(0.0f, 1.0f), Vector3f(-1.0f, 0.0f, 0.0f),
							Vector3f(-0.5f, -0.5f, -0.5f), Vector2f(0.0f, 1.0f), Vector3f(-1.0f, 0.0f, 0.0f),
							Vector3f(-0.5f, -0.5f,  0.5f), Vector2f(0.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f),
							Vector3f(-0.5f,  0.5f,  0.5f), Vector2f(1.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f),
							
							
							Vector3f(0.5f,  0.5f,  0.5f), Vector2f(1.0f, 0.0f),  Vector3f(1.0f, 0.0f, 0.0f),
							Vector3f(0.5f,  0.5f, -0.5f), Vector2f(1.0f, 1.0f),	 Vector3f(1.0f, 0.0f, 0.0f),
							Vector3f(0.5f, -0.5f, -0.5f), Vector2f(0.0f, 1.0f),	 Vector3f(1.0f, 0.0f, 0.0f),
							Vector3f(0.5f, -0.5f, -0.5f), Vector2f(0.0f, 1.0f),	 Vector3f(1.0f, 0.0f, 0.0f),
							Vector3f(0.5f, -0.5f,  0.5f), Vector2f(0.0f, 0.0f),	 Vector3f(1.0f, 0.0f, 0.0f),
							Vector3f(0.5f,  0.5f,  0.5f), Vector2f(1.0f, 0.0f),	 Vector3f(1.0f, 0.0f, 0.0f),
	
							
							Vector3f(-0.5f, -0.5f, -0.5f), Vector2f(0.0f, 1.0f), Vector3f(0.0f, -1.0f, 0.0f),
							Vector3f( 0.5f, -0.5f, -0.5f), Vector2f(1.0f, 1.0f), Vector3f(0.0f, -1.0f, 0.0f),
							Vector3f( 0.5f, -0.5f,  0.5f), Vector2f(1.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f),
							Vector3f( 0.5f, -0.5f,  0.5f), Vector2f(1.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f),
							Vector3f(-0.5f, -0.5f,  0.5f), Vector2f(0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f),
							Vector3f(-0.5f, -0.5f, -0.5f), Vector2f(0.0f, 1.0f), Vector3f(0.0f, -1.0f, 0.0f),
							
							
							Vector3f(-0.5f, 0.5f, -0.5f), Vector2f(0.0f, 1.0f), Vector3f(0.0f, 11.0f, 0.0f),
							Vector3f( 0.5f, 0.5f, -0.5f), Vector2f(1.0f, 1.0f),	Vector3f(0.0f, 11.0f, 0.0f),
							Vector3f( 0.5f, 0.5f,  0.5f), Vector2f(1.0f, 0.0f),	Vector3f(0.0f, 11.0f, 0.0f),
							Vector3f( 0.5f, 0.5f,  0.5f), Vector2f(1.0f, 0.0f), Vector3f(0.0f, 11.0f, 0.0f),
							Vector3f(-0.5f, 0.5f,  0.5f), Vector2f(0.0f, 0.0f),	Vector3f(0.0f, 11.0f, 0.0f),
							Vector3f(-0.5f, 0.5f, -0.5f), Vector2f(0.0f, 1.0f),	Vector3f(0.0f, 11.0f, 0.0f)		};

	GLuint m_Indices[36] = { 0, 1, 2,
							 3, 4, 5,
							
							 6, 7, 8,
							 9, 10, 11,

							 12, 13, 14,
							 15, 16, 17,

							 18, 19, 20,
							 21, 22, 23,

							 24, 25, 26,
							 27, 28, 29,
							
							 30, 31, 32,
							 33, 34, 35	};
};



class VBO final {
public:
	VBO() = delete;
	VBO(const void* data, const GLuint size) noexcept {
		GLCall(glGenBuffers(1, &m_ID));
		Bind();
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
		ActivatePositionAttribute();
		ActivateTextureCoordinatesAttribute();
		ActivateNormalAttribute();
		Unbind(); 
	}
	~VBO() {
		GLCall(glDeleteBuffers(1, &m_ID));
	}

	VBO(const VBO&) = delete;
	VBO& operator=(const VBO&) = delete;

	VBO(VBO&&) = delete;
	VBO& operator=(VBO&&) = delete;

public:
	void Bind() const noexcept {
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
	}
	void Unbind() const noexcept {
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

private:
	void ActivatePositionAttribute() const noexcept {
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
		GLCall(glEnableVertexAttribArray(0));
	}
	void ActivateTextureCoordinatesAttribute() const noexcept {
		GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3f))));
		GLCall(glEnableVertexAttribArray(1));
	}
	void ActivateNormalAttribute() const noexcept {
		GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3f) + sizeof(Vector2f))));
		GLCall(glEnableVertexAttribArray(2));
	}

private:
	GLuint m_ID;
};


class VAO final {
public:
	VAO() noexcept 
		:	m_ID(0)
	{
		GLCall(glGenVertexArrays(1, &m_ID));
	}
	~VAO() {
		GLCall(glDeleteBuffers(1, &m_ID));
	}

	//Otherwise the copy will delete the buffer
	VAO(const VAO& other) noexcept = delete;
	VAO& operator=(const VAO& other) noexcept = delete;

	VAO(VAO&& other) noexcept {
		*this = std::move(other);
	}
	VAO& operator=(VAO&& other) noexcept {
		if (this == &other)
			return *this;
		else {

			this->m_ID = other.m_ID;
			other.m_ID = 0; // 0 is used by opengl to unbind. It is not valid. This would need to change for other rendering APIs probably. Update: VAOs are only for opengl...

			return *this;
		}
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
	EBO(const GLuint* data, const GLuint size, const GLuint count) noexcept { //Could make smaller ones with shorts for better perf
		GLCall(glGenBuffers(1, &m_ID));
		Bind();
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
		Unbind();
		m_Count = count;
	}
	~EBO() {
		GLCall(glDeleteBuffers(1, &m_ID));
	}

	EBO(const EBO&) = delete;
	EBO& operator=(const EBO&) = delete;

	EBO(EBO&&) = delete;
	EBO& operator=(EBO&&) = delete;

public:
	void Bind() const noexcept {
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
	}
	void Unbind() const noexcept {
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	inline GLuint GetCount() const noexcept { return m_Count; };

private:
	GLuint m_ID;
	GLuint m_Count;
};