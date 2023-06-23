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
};
struct Vector4f {
	float X, Y, Z, W;
};

//Move somewhere else!
struct Color {
	float R, G, B = 0.0f;
	float A = 1.0f;
};
