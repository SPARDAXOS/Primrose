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


class Transform final {
public:
	explicit Transform() = default;
	Transform(Vector3f position, Vector3f rotation, Vector3f scale) noexcept
		: m_Position(position), m_Rotation(rotation), m_Scale(scale)
	{
	}
	Transform(Vector3f position, Vector3f rotation) noexcept
		: m_Position(position), m_Rotation(rotation), m_Scale(Vector3f(1.0f))
	{
	}
	Transform(Vector3f position) noexcept
		: m_Position(position), m_Rotation(Vector3f(0.0f)), m_Scale(Vector3f(1.0f))
	{
	}

	~Transform() = default;

	Transform(const Transform& other) noexcept {
		*this = other;
	}
	Transform& operator=(const Transform& other) noexcept {
		if (this == &other) {
			return *this;
		}
		else {
			this->m_Position = other.m_Position;
			this->m_Rotation = other.m_Rotation;
			this->m_Scale = other.m_Scale;
			return *this;
		}
	}

	Transform(Transform&& other) noexcept {
		*this = std::move(other);
	}
	Transform& operator=(Transform&& other) noexcept {
		if (this == &other) {
			return *this;
		}
		else {
			this->m_Position = std::move(other.m_Position);
			this->m_Rotation = std::move(other.m_Rotation);
			this->m_Scale = std::move(other.m_Scale);
			return *this;
		}
	}

public:
	void UpdateMatrix() {
		m_Matrix = glm::mat4(1.0f);

		m_Matrix = glm::translate(m_Matrix, glm::vec3(m_Position.m_X, m_Position.m_Y, m_Position.m_Z));

		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_X), glm::vec3(1.0f, 0.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Z), glm::vec3(0.0f, 0.0f, 1.0f));

		m_Matrix = glm::scale(m_Matrix, glm::vec3(m_Scale.m_X, m_Scale.m_Y, m_Scale.m_Z));
	}
	[[nodiscard]] inline glm::mat4 GetMatrix() const noexcept { return m_Matrix; };

public:
	Vector3f m_Position { 0.0f };
	Vector3f m_Rotation { 0.0f };
	Vector3f m_Scale    { 1.0f };

private:
	glm::mat4 m_Matrix;
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
	EBO(const GLuint* data, const GLuint size) noexcept { //Could make smaller ones with shorts for better perf
		GLCall(glGenBuffers(1, &m_ID));
		Bind();
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
		Unbind();
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

private:
	GLuint m_ID;
};