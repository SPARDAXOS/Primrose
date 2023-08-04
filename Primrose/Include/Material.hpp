#include "Utility.hpp"
#include "Texture2D.hpp"


class Material {
public:

	explicit Material() = delete;
	Material(Asset& asset) 
		: m_Asset(&asset)
	{
	}
	~Material() = default;

	Material(const Material& other) noexcept {
		*this = other;
	}
	Material& operator=(const Material& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Diffuse = other.m_Diffuse;
			this->m_Ambient = other.m_Ambient;
			this->m_Specular = other.m_Specular;

			this->m_SpecularStrength = other.m_SpecularStrength;
			this->m_SpecularShininess = other.m_SpecularShininess;

			this->m_AmbientStrength = other.m_AmbientStrength;
			//NOTE: Cant have 2 materials pointing to the same asset.

			return *this;
		}
	}

	Material(Material&& other) noexcept {
		*this = std::move(other);
	}
	Material& operator=(Material&& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Diffuse = std::move(other.m_Diffuse);
			this->m_Ambient = std::move(other.m_Ambient);
			this->m_Specular = std::move(other.m_Specular);
			this->m_SpecularStrength = std::move(other.m_SpecularStrength);
			this->m_SpecularShininess = std::move(other.m_SpecularShininess);
			this->m_AmbientStrength = std::move(other.m_AmbientStrength);

			this->m_Asset = std::move(other.m_Asset);
			return *this;
		}
	}

	bool operator==(const Material& other) const noexcept {
		//NOTE: This is not efficient and scales badly. Maybe the loaded provides unique ids? and i compare that instead.
		if (m_Diffuse != other.m_Diffuse)
			return false;
		else if (m_Ambient != other.m_Ambient)
			return false;
		else if (m_Specular != other.m_Specular)
			return false;
		else if (m_SpecularStrength != other.m_SpecularStrength)
			return false;
		else if (m_SpecularShininess != other.m_SpecularShininess)
			return false;
		else if (m_AmbientStrength != other.m_AmbientStrength)
			return false;
		else
			return true;
	}
	bool operator!=(const Material& other) const noexcept {
		if (!(*this == other))
			return true;
		else
			return false;
	}

public:
	Asset* GetAsset() const noexcept { return m_Asset; }

public:
	Texture2D* m_Diffuse	{ nullptr };
	Texture2D* m_Ambient	{ nullptr };
	Texture2D* m_Specular	{ nullptr };

	float m_AmbientStrength = 1.0f;

	float m_SpecularStrength = 0.5f;
	int32 m_SpecularShininess = 32;

private:
	Asset* m_Asset	{ nullptr };
};