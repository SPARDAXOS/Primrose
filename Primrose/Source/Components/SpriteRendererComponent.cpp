#include "Components/SpriteRendererComponent.hpp"
#include "GameObject.hpp"

SpriteRenderer::SpriteRenderer(GameObject& owner, int64 ownerID) noexcept
	: ComponentBase(owner, ownerID)
{
	//TODO: Clean this up. This looks messy!
	m_VAO = new VAO;
	m_VAO->Bind();

	m_VBO = new VBO(m_Primitive.m_Data, sizeof(m_Primitive.m_Data));
	m_EBO = new EBO(m_Primitive.m_Indices, sizeof(m_Primitive.m_Indices), sizeof(m_Primitive.m_Indices) / sizeof(GLuint));

	m_VBO->Bind();
	m_EBO->Bind();

	m_VAO->Unbind();
	m_VBO->Unbind();
	m_EBO->Unbind();
}
SpriteRenderer::~SpriteRenderer() {
	delete m_VAO;
	delete m_VBO;
	delete m_EBO;
}