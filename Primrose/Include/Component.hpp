#pragma once
#include "Utility.hpp"
#include "Math.hpp"

constexpr uint32 INVALID_ID = 0b00000000000000000000000000000000;
constexpr uint32 SPRITE_COMPONENT_ID = 0b00000000000000000000000000000001;

//Move somewhere else or get rid of it
constexpr uint32 MAIN_SCENE_OBJECT_ID = 0b00000000000000000000000000000001;


class SpriteRenderer;

class ComponentBase {
protected:
	bool m_Enabled{ true };
	uint64 m_OwnerID;

public:
	template<typename T>
	static uint32 GetComponentID() {
		static_assert(std::is_base_of_v<ComponentBase, T>);


		//You add new components here
		if (std::is_object_v(T, SpriteRenderer))
			return SPRITE_COMPONENT_ID;

		PrintMessage("[Warning] Invalid component sent to HasComponent()");
		return INVALID_ID;
	}
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
	Vector3f m_Position{ 0.0f };
	Vector3f m_Rotation{ 0.0f };
	Vector3f m_Scale{ 1.0f };

private:
	glm::mat4 m_Matrix;
};

class SpriteRenderer final : ComponentBase {





};