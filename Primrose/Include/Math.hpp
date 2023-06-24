#pragma once


struct Vector2f {
	float X, Y;

	Vector2f() = delete;
	Vector2f(float X, float Y) noexcept
		: X(X), Y(Y)
	{
	}

	Vector2f operator*(const Vector2f& rhs) noexcept {
		return Vector2f(X * rhs.X, Y * rhs.Y);
	}
	Vector2f operator/(const Vector2f& rhs) noexcept {
		return Vector2f(X / rhs.X, Y / rhs.Y);
	}
	Vector2f operator+(const Vector2f& rhs) noexcept {
		return Vector2f(X + rhs.X, Y + rhs.Y);
	}
	Vector2f operator-(const Vector2f& rhs) noexcept {
		return Vector2f(X - rhs.X, Y - rhs.Y);
	}

	bool operator==(const Vector2f& rhs) const noexcept {
		if (X != rhs.X)
			return false;
		else if (Y != rhs.Y)
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
	float X, Y, Z;

	Vector3f() = delete;
	Vector3f(float X, float Y, float Z) noexcept
		: X(X), Y(Y), Z(Z)
	{
	}

	Vector3f operator*(const Vector3f& rhs) noexcept {
		return Vector3f(X * rhs.X, Y * rhs.Y, Z * rhs.Z);
	}
	Vector3f operator/(const Vector3f& rhs) noexcept {
		return Vector3f(X / rhs.X, Y / rhs.Y, Z / rhs.Z);
	}
	Vector3f operator+(const Vector3f& rhs) noexcept {
		return Vector3f(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
	}
	Vector3f operator-(const Vector3f& rhs) noexcept {
		return Vector3f(X - rhs.X, Y - rhs.Y, Z - rhs.Z);
	}

	bool operator==(const Vector3f& rhs) const noexcept {
		if (X != rhs.X)
			return false;
		else if (Y != rhs.Y)
			return false;
		else if (Z != rhs.Z)
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
struct Position {
	float X, Y, Z;
};
struct Color {
	float R, G, B = 0.0f;
	float A = 1.0f;
};
struct TextureCordinates {
	float X, Y;
};

namespace Colors {
	static Color Black   = { 0.0f, 0.0f, 0.0f, 1.0f };
	static Color White   = { 1.0f, 1.0f, 1.0f, 1.0f };
	static Color Red     = { 1.0f, 0.0f, 0.0f, 1.0f };
	static Color Green   = { 0.0f, 1.0f, 0.0f, 1.0f };
	static Color Blue    = { 0.0f, 0.0f, 1.0f, 1.0f };
	static Color Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

}
