#pragma once
#include "Math.hpp"


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
	inline void ClearMatrix() { m_Matrix = glm::mat4(1.0f); }
	void UpdateMatrix() {

		//Matrix for each maybe?
		m_Matrix = glm::translate(m_Matrix, glm::vec3(m_Position.m_X, m_Position.m_Y, m_Position.m_Z));

		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_X), glm::vec3(1.0f, 0.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Z), glm::vec3(0.0f, 0.0f, 1.0f));
		
		m_Matrix = glm::scale(m_Matrix, glm::vec3(m_Scale.m_X, m_Scale.m_Y, m_Scale.m_Z));


		//glm::mat4 TranlationMatrix = glm::mat4(1.0f);
		//TranlationMatrix = glm::translate(TranlationMatrix, glm::vec3(m_Position.m_X, m_Position.m_Y, m_Position.m_Z));

		//glm::mat4 RotationMatrix = glm::mat4(1.0f);
		//RotationMatrix = glm::rotate(RotationMatrix, glm::radians(m_Rotation.m_X), glm::vec3(1.0f, 0.0f, 0.0f));
		//RotationMatrix = glm::rotate(RotationMatrix, glm::radians(m_Rotation.m_Y), glm::vec3(0.0f, 1.0f, 0.0f));
		//RotationMatrix = glm::rotate(RotationMatrix, glm::radians(m_Rotation.m_Z), glm::vec3(0.0f, 0.0f, 1.0f));

		//glm::mat4 ScaleMatrix = glm::mat4(1.0f);
		//ScaleMatrix = glm::scale(ScaleMatrix, glm::vec3(m_Scale.m_X, m_Scale.m_Y, m_Scale.m_Z));

		//m_Matrix = TranlationMatrix * RotationMatrix * ScaleMatrix;
	}

	[[nodiscard]] inline glm::mat4& GetMatrix() noexcept { return m_Matrix; };



public:
	Vector3f m_Position{ 0.0f };
	Vector3f m_Rotation{ 0.0f };
	Vector3f m_Scale{ 1.0f };

private:
	glm::mat4 m_Matrix;
};