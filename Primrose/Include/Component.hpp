#pragma once
#include "Utility.hpp"
#include "Math.hpp"
#include "GraphicsResources.hpp"
#include "TextureStorage.hpp"

constexpr uint32 INVALID_ID = 0b00000000000000000000000000000000;
constexpr uint32 SPRITE_COMPONENT_ID = 0b00000000000000000000000000000001;

//Move somewhere else or get rid of it
constexpr uint32 MAIN_SCENE_OBJECT_ID = 0b00000000000000000000000000000001;


class SpriteRenderer;

class ComponentBase {
public:
	ComponentBase() = delete;
	ComponentBase(uint64 ownerID) noexcept 
		: m_OwnerID(ownerID)
	{
	}


public:
	inline void SetEnabled(bool state) noexcept { m_Enabled = state; };
	inline bool GetEnabled() const noexcept { return m_Enabled; };
	inline uint64 GetOwnerID() const noexcept { return m_OwnerID; };


protected:
	bool m_Enabled{ true };
	uint64 m_OwnerID;
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
	[[nodiscard]] inline glm::mat4 GetMatrix() { UpdateMatrix(); return m_Matrix; };

private:
	void UpdateMatrix() {
		m_Matrix = glm::mat4(1.0f);

		m_Matrix = glm::translate(m_Matrix, glm::vec3(m_Position.m_X, m_Position.m_Y, m_Position.m_Z));

		m_Matrix = glm::scale(m_Matrix, glm::vec3(m_Scale.m_X, m_Scale.m_Y, m_Scale.m_Z));

		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_X), glm::vec3(1.0f, 0.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

public:
	Vector3f m_Position{ 0.0f };
	Vector3f m_Rotation{ 0.0f };
	Vector3f m_Scale{ 1.0f };

private:
	glm::mat4 m_Matrix;
};

class SpriteRenderer final : public ComponentBase {
public:
	SpriteRenderer() = delete;
	SpriteRenderer(uint64 ownerID) noexcept 
		:	ComponentBase(ownerID)
	{
		//TODO: Clean this up. This looks messy!
		m_VAO = new VAO;
		m_VAO->Bind();

		m_VBO = new VBO(m_Primitive.m_Data, sizeof(m_Primitive.m_Data));
		m_EBO = new EBO(m_Primitive.m_Indices, sizeof(m_Primitive.m_Indices));

		m_VBO->Bind();
		m_EBO->Bind();

		m_VAO->Unbind();
		m_VBO->Unbind();
		m_EBO->Unbind();
	}
	~SpriteRenderer() {
		delete m_VAO;
		delete m_VBO;
		delete m_EBO;
	}


	//For now it is not possbile to move or copy components
	SpriteRenderer(const SpriteRenderer& other) = delete;
	SpriteRenderer& operator=(const SpriteRenderer& other) = delete;

	SpriteRenderer(SpriteRenderer&& other) = delete;
	SpriteRenderer& operator=(SpriteRenderer&& other) = delete;

public:
	inline void SetSprite(Texture2D* sprite) noexcept { m_Sprite = sprite; }

	inline VAO* GetVAO() const noexcept { return m_VAO; }
	inline Texture2D* GetSprite() const noexcept { return m_Sprite; }



private:
	Texture2D* m_Sprite = nullptr;

private:
	Square m_Primitive;
	VAO* m_VAO = nullptr;
	VBO* m_VBO = nullptr;
	EBO* m_EBO = nullptr;
};


namespace Components {
	//You add new components template specializations here
	template<typename T>
	inline uint32 GetComponentID() noexcept;
	template<>
	inline uint32 GetComponentID<SpriteRenderer>() noexcept { return SPRITE_COMPONENT_ID; }

	//TODO: Add Custom Component which is basically a component with no code that can be customized.
}