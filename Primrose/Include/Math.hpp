#pragma once
#pragma warning(push, 0)
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"
#pragma warning(pop)

struct Vector2f {
	float m_X, m_Y;

	Vector2f() = delete;
	Vector2f(float value) noexcept
		: m_X(value), m_Y(value)
	{
	}
	Vector2f(float X, float Y) noexcept
		: m_X(X), m_Y(Y)
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

	Vector2f operator*(const Vector2f& rhs) noexcept {
		return Vector2f(m_X * rhs.m_X, m_Y * rhs.m_Y);
	}
	Vector2f operator/(const Vector2f& rhs) noexcept {
		return Vector2f(m_X / rhs.m_X, m_Y / rhs.m_Y);
	}
	Vector2f operator+(const Vector2f& rhs) noexcept {
		return Vector2f(m_X + rhs.m_X, m_Y + rhs.m_Y);
	}
	Vector2f operator-(const Vector2f& rhs) noexcept {
		return Vector2f(m_X - rhs.m_X, m_Y - rhs.m_Y);
	}

	Vector2f operator*(const float& rhs) noexcept {
		return Vector2f(m_X * rhs, m_Y * rhs);
	}
	Vector2f operator/(const float& rhs) noexcept {
		return Vector2f(m_X / rhs, m_Y / rhs);
	}
	Vector2f operator+(const float& rhs) noexcept {
		return Vector2f(m_X + rhs, m_Y + rhs);
	}
	Vector2f operator-(const float& rhs) noexcept {
		return Vector2f(m_X - rhs, m_Y - rhs);
	}

	bool operator==(const Vector2f& rhs) const noexcept {
		if (m_X != rhs.m_X)
			return false;
		else if (m_Y != rhs.m_Y)
			return false;
		else
			return true;
	}
	bool operator!=(const Vector2f& rhs) const noexcept {
		if (!(*this == rhs))
			return true;
		else
			return false;
	}
};
struct Vector3f {
	float m_X, m_Y, m_Z;

	Vector3f() = delete;
	Vector3f(float value) noexcept
		: m_X(value), m_Y(value), m_Z(value)
	{
	}
	Vector3f(float x, float y, float z) noexcept
		: m_X(x), m_Y(y), m_Z(z)
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

	Vector3f operator*(const Vector3f& rhs) noexcept {
		return Vector3f(m_X * rhs.m_X, m_Y * rhs.m_Y, m_Z * rhs.m_Z);
	}
	Vector3f operator/(const Vector3f& rhs) noexcept {
		return Vector3f(m_X / rhs.m_X, m_Y / rhs.m_Y, m_Z / rhs.m_Z);
	}
	Vector3f operator+(const Vector3f& rhs) noexcept {
		return Vector3f(m_X + rhs.m_X, m_Y + rhs.m_Y, m_Z + rhs.m_Z);
	}
	Vector3f operator-(const Vector3f& rhs) noexcept {
		return Vector3f(m_X - rhs.m_X, m_Y - rhs.m_Y, m_Z - rhs.m_Z);
	}

	Vector3f operator*(const float& rhs) noexcept {
		return Vector3f(m_X * rhs, m_Y * rhs, m_Z * rhs);
	}
	Vector3f operator/(const float& rhs) noexcept {
		return Vector3f(m_X / rhs, m_Y / rhs, m_Z / rhs);
	}
	Vector3f operator+(const float& rhs) noexcept {
		return Vector3f(m_X + rhs, m_Y + rhs, m_Z + rhs);
	}
	Vector3f operator-(const float& rhs) noexcept {
		return Vector3f(m_X - rhs, m_Y - rhs, m_Z - rhs);
	}

	bool operator==(const Vector3f& rhs) const noexcept {
		if (m_X != rhs.m_X)
			return false;
		else if (m_Y != rhs.m_Y)
			return false;
		else if (m_Z != rhs.m_Z)
			return false;
		else
			return true;
	}
	bool operator!=(const Vector3f& rhs) const noexcept {
		if (!(*this == rhs))
			return true;
		else
			return false;
	}
};
struct Vector4f {
	float X, Y, Z, W;
};

//Move somewhere else!
struct Position { //Remove this and replace it with vector 3 in the vertices. Add redifinition or alias if its for clairties sake but i dont think its needed
	float X, Y, Z;
};
struct Color {
	float m_R, m_G, m_B = 0.0f;
	float m_A = 1.0f;
};
struct TextureUV{
	float m_U, m_V;
};

namespace Colors {
	static Color Black    = { 0.0f, 0.0f, 0.0f, 1.0f };
	static Color White    = { 1.0f, 1.0f, 1.0f, 1.0f };
	static Color Red      = { 1.0f, 0.0f, 0.0f, 1.0f };
	static Color Green    = { 0.0f, 1.0f, 0.0f, 1.0f };
	static Color Blue     = { 0.0f, 0.0f, 1.0f, 1.0f };
	static Color Magenta  = { 1.0f, 0.0f, 1.0f, 1.0f };
	static Color PaleGreen = { 0.2f, 0.3f, 0.3f, 1.0f };
}
