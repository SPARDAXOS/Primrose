#pragma once
#include <GLAD/glad/glad.h>

//TODO: Use bits instead since these are in NDC so -1.0 -> 1.0

struct Vertex {
	float X, Y, Z;
};

struct Triangle {
	Vertex m_Data[3] = {	1.0f, 0.0f, 0.0f,
						   -1.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f	};
};

struct VBO {
	VBO() noexcept {
		glGenBuffers(1, &m_BufferID);
	}

	GLuint m_BufferID;
};