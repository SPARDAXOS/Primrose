#pragma once
#pragma warning(push, 0)
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include <math.h>
#pragma warning(pop)

struct Vector2f {
	float m_X, m_Y;

	Vector2f() noexcept
		: m_X(0.0f), m_Y(0.0f)
	{
	}
	Vector2f(float value) noexcept
		: m_X(value), m_Y(value)
	{
	}
	Vector2f(float X, float Y) noexcept
		: m_X(X), m_Y(Y)
	{
	}
	Vector2f(glm::vec2 vector) noexcept
		: m_X(vector.x), m_Y(vector.y)
	{
	}

	~Vector2f() = default;
	Vector2f(const Vector2f& other) noexcept {
		*this = other;
	}
	Vector2f& operator=(const Vector2f& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_X = other.m_X;
			this->m_Y = other.m_Y;
			return *this;
		}
	}
	Vector2f(Vector2f&& other) noexcept {
		*this = std::move(other);
	}
	Vector2f& operator=(Vector2f&& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_X = std::move(other.m_X);
			this->m_Y = std::move(other.m_Y);
			return *this;
		}
	}

	Vector2f operator*(const Vector2f& other) noexcept {
		return Vector2f(m_X * other.m_X, m_Y * other.m_Y);
	}
	Vector2f operator/(const Vector2f& other) noexcept {
		return Vector2f(m_X / other.m_X, m_Y / other.m_Y);
	}
	Vector2f operator+(const Vector2f& other) noexcept {
		return Vector2f(m_X + other.m_X, m_Y + other.m_Y);
	}
	Vector2f operator-(const Vector2f& other) noexcept {
		return Vector2f(m_X - other.m_X, m_Y - other.m_Y);
	}

	Vector2f operator*(const float& other) noexcept {
		return Vector2f(m_X * other, m_Y * other);
	}
	Vector2f operator/(const float& other) noexcept {
		return Vector2f(m_X / other, m_Y / other);
	}
	Vector2f operator+(const float& other) noexcept {
		return Vector2f(m_X + other, m_Y + other);
	}
	Vector2f operator-(const float& other) noexcept {
		return Vector2f(m_X - other, m_Y - other);
	}

	void operator+=(const Vector2f& other) noexcept {
		m_X += other.m_X;
		m_Y += other.m_Y;
	}

	bool operator==(const Vector2f& other) const noexcept {
		if (m_X != other.m_X)
			return false;
		else if (m_Y != other.m_Y)
			return false;
		else
			return true;
	}
	bool operator!=(const Vector2f& other) const noexcept {
		if (!(*this == other))
			return true;
		else
			return false;
	}

	inline float Length() const noexcept {
		return sqrtf(((m_X * m_X), (m_Y * m_Y)));
	}
	inline void Normalize() noexcept {
		const float Length = this->Length();
		m_X /= Length;
		m_Y /= Length;
	}
};
struct Vector3f {
	float m_X, m_Y, m_Z;

	Vector3f() noexcept
		: m_X(0.0f), m_Y(0.0f), m_Z(0.0f)
	{
	}
	Vector3f(float value) noexcept
		: m_X(value), m_Y(value), m_Z(value)
	{
	}
	Vector3f(float x, float y, float z) noexcept
		: m_X(x), m_Y(y), m_Z(z)
	{
	}
	Vector3f(glm::vec3 vector) noexcept
		: m_X(vector.x), m_Y(vector.y), m_Z(vector.z)
	{
	}

	~Vector3f() = default;
	Vector3f(const Vector3f& other) noexcept {
		*this = other;
	}
	Vector3f& operator=(const Vector3f& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_X = other.m_X;
			this->m_Y = other.m_Y;
			this->m_Z = other.m_Z;
			return *this;
		}
	}

	Vector3f(Vector3f&& other) noexcept {
		*this = std::move(other);
	}
	Vector3f& operator=(Vector3f&& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_X = std::move(other.m_X);
			this->m_Y = std::move(other.m_Y);
			this->m_Z = std::move(other.m_Z);
			return *this;
		}
	}

	Vector3f operator*(const Vector3f& other) noexcept {
		return Vector3f(m_X * other.m_X, m_Y * other.m_Y, m_Z * other.m_Z);
	}
	Vector3f operator/(const Vector3f& other) noexcept {
		return Vector3f(m_X / other.m_X, m_Y / other.m_Y, m_Z / other.m_Z);
	}
	Vector3f operator+(const Vector3f& other) noexcept {
		return Vector3f(m_X + other.m_X, m_Y + other.m_Y, m_Z + other.m_Z);
	}
	Vector3f operator-(const Vector3f& other) noexcept {
		return Vector3f(m_X - other.m_X, m_Y - other.m_Y, m_Z - other.m_Z);
	}

	Vector3f operator*(const float& other) noexcept {
		return Vector3f(m_X * other, m_Y * other, m_Z * other);
	}
	Vector3f operator/(const float& other) noexcept {
		return Vector3f(m_X / other, m_Y / other, m_Z / other);
	}
	Vector3f operator+(const float& other) noexcept {
		return Vector3f(m_X + other, m_Y + other, m_Z + other);
	}
	Vector3f operator-(const float& other) noexcept {
		return Vector3f(m_X - other, m_Y - other, m_Z - other);
	}

	void operator+=(const Vector3f& other) noexcept {
		m_X += other.m_X;
		m_Y += other.m_Y;
		m_Z += other.m_Z;
	}

	bool operator==(const Vector3f& other) const noexcept {
		if (m_X != other.m_X)
			return false;
		else if (m_Y != other.m_Y)
			return false;
		else if (m_Z != other.m_Z)
			return false;
		else
			return true;
	}
	bool operator!=(const Vector3f& other) const noexcept {
		if (!(*this == other))
			return true;
		else
			return false;
	}


	inline float Length() const noexcept {
		return sqrtf(((m_X * m_X), (m_Y * m_Y), (m_Z * m_Z)));
	}
	inline void Normalize() noexcept {
		const float Length = this->Length();
		m_X /= Length;
		m_Y /= Length;
		m_Z /= Length;
	}
};
struct Vector4f {
	float X, Y, Z, W;
};

//Move somewhere else!
struct Color {
	float m_R, m_G, m_B = 0.0f;
	float m_A = 1.0f;
};


namespace Colors {
	constexpr static Color Black     = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr static Color White     = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr static Color Red       = { 1.0f, 0.0f, 0.0f, 1.0f };
	constexpr static Color Green     = { 0.0f, 1.0f, 0.0f, 1.0f };
	constexpr static Color Blue      = { 0.0f, 0.0f, 1.0f, 1.0f };
	constexpr static Color Magenta   = { 1.0f, 0.0f, 1.0f, 1.0f };
	constexpr static Color PaleGreen = { 0.2f, 0.3f, 0.3f, 1.0f };
}
