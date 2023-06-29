#pragma once
#include <GLAD/glad/glad.h>
#include "GLErrorHandling.hpp"
#include "ShaderProgram.hpp"


//TODO: Use bits instead since these are in NDC so -1.0 -> 1.0

struct Vertex {
	Position m_Position;
	Color m_Color;
	TextureUV m_TextureCoordinates;
};

struct Triangle {
	Vertex m_Data[3] = {	0.5f, -0.5f, 0.0f,
						   -0.5f, -0.5f, 0.0f,
							0.0f,  0.5f, 0.0f	};

	GLuint m_Indices[3] = {	0, 1, 2	};

};
struct Square {
	Vertex m_Data[4] = {	Position( 0.5f, -0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
							Position(-0.5f, -0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
							Position( 0.5f,  0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),
							Position(-0.5f,  0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f)	  };
	
	GLuint m_Indices[6] = {	0, 2, 3,
							1, 0, 3	};
};
struct Cube {
	//Vertex m_Data[24] = {   //Front
	//						Position( 0.5f, -0.5f, 0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
	//						Position(-0.5f, -0.5f, 0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
	//						Position( 0.5f,  0.5f, 0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),
	//						Position(-0.5f,  0.5f, 0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f),

	//						//Back
	//						Position( 0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
	//						Position(-0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
	//						Position( 0.5f,  0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),
	//						Position(-0.5f,  0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f),
	//
	//						//Right
	//						Position(0.5f, -0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
	//						Position(0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
	//						Position(0.5f,  0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),
	//						Position(0.5f,  0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f), 
	//						
	//						//Left
	//						Position(-0.5f, -0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
	//						Position(-0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
	//						Position(-0.5f,  0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),
	//						Position(-0.5f,  0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f),
	//
	//						//Up
	//						Position(-0.5f, 0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
	//						Position(-0.5f, 0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
	//						Position( 0.5f, 0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),
	//						Position( 0.5f, 0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f),
	//						
	//						//Down
	//						Position(-0.5f, -0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
	//						Position(-0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
	//						Position( 0.5f, -0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),
	//						Position( 0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f) };

	Vertex m_Data[8] = {   //FrontSide
							Position(-0.5f, -0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
							Position( 0.5f, -0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
							Position(-0.5f,  0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f),
							Position( 0.5f,  0.5f,  0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f),

							//BackSide
							Position(-0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 0.0f),
							Position( 0.5f, -0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 0.0f),
							Position(-0.5f,  0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(0.0f, 1.0f),
							Position( 0.5f,  0.5f, -0.5f), Color(1.0f, 1.0f, 1.0f, 1.0f), TextureUV(1.0f, 1.0f)	  };


	GLuint m_Indices[36] = { 2, 6, 7,
							 2, 3, 7,
							
							 0, 4, 5,
							 0, 1, 5,

							 0, 2, 6,
							 0, 4, 6,

							 1, 3, 7,
							 1, 5, 7,

							 0, 2, 3,
							 0, 1, 3,
							
							 4, 6, 7,
							 4, 5, 7	};
};



class VBO final {
public:
	VBO() = delete;
	VBO(const void* data, const GLuint size) noexcept {
		GLCall(glGenBuffers(1, &m_ID));
		Bind();
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
		ActivatePositionAttribute();
		ActivateColorAttribute();
		ActivateTextureCoordinatesAttribute();
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
	void ActivateColorAttribute() const noexcept {
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position))));
		GLCall(glEnableVertexAttribArray(1));
	}
	void ActivateTextureCoordinatesAttribute() const noexcept {
		GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color))));
		GLCall(glEnableVertexAttribArray(2));
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

	VAO(const VAO&) = delete;
	VAO& operator=(const VAO&) = delete;

	VAO(VAO&&) = delete;
	VAO& operator=(VAO&&) = delete;

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