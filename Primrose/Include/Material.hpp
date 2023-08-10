#include "Utility.hpp"
#include "Texture2D.hpp"


class Material {
public:

	explicit Material() = delete;
	Material(Asset& asset, AssetManager& assetManager) noexcept 
		: m_Asset(&asset), m_AssetManagerReference(&assetManager)
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
	Asset& GetAsset() const noexcept { return *m_Asset; }

public:
	[[nodiscard]] inline bool SerializeToFile(char*& buffer, size_t& size) const {
		
		size_t Cursor = 0;

		//IMPORTANT NOTE: This way, reordering the member variables wouldnt break things.
		//-Just gotta make sure to deserialize the data in the same order as it gets serialized.
		
		//Diffuse
		if (m_Diffuse != nullptr) {
			std::string Name = m_Diffuse->GetName().data();
			Name += "\n";
			std::memcpy(buffer + Cursor, Name.data(), Name.size());
			Cursor += Name.size();
		}
		else {
			const char* test = "None\n";
			std::memcpy(buffer, test, 5);
			Cursor += 5;
		}

		//Ambient
		if (m_Ambient != nullptr) {
			std::string Name = m_Ambient->GetName().data();
			Name += "\n";
			std::memcpy(buffer + Cursor, Name.data(), Name.size());
			Cursor += Name.size();
		}
		else {
			std::memcpy(buffer + Cursor, "None\n", 5);
			Cursor += 5;
		}

		//Specular
		if (m_Specular != nullptr) {
			std::string Name = m_Specular->GetName().data();
			Name += "\n";
			std::memcpy(buffer + Cursor, Name.data(), Name.size());
			Cursor += Name.size();
		}
		else {
			std::memcpy(buffer + Cursor, "None\n", 5);
			Cursor += 5;
		}

		//NOTE: Unsure here cause it seems like there are 2 bytes that are skipped at the start or its fine!
		std::memcpy(buffer + Cursor, &m_AmbientStrength, 4);
		Cursor += 4;

		std::memcpy(buffer + Cursor, &m_SpecularStrength, 4);
		Cursor += 4;

		std::memcpy(buffer + Cursor, &m_SpecularShininess, 4);
		Cursor += 4;

		size = Cursor;
		return true;
	}
	[[nodiscard]] inline bool SerializeFromFile(char*& buffer) {

		std::istringstream StringStream(buffer);

		std::string TextureName;
		Texture2D* Texture_ptr = nullptr;
		size_t Cursor = 0;

		//Diffuse
		std::getline(StringStream, TextureName);
		if (TextureName != "None") {
			if (!m_AssetManagerReference->RequestTexture2D(TextureName, Texture_ptr)) {
				//Print error message! Get it from asset manager or something?
			}
			m_Diffuse = Texture_ptr;
			Cursor += TextureName.size();
		}
		else
			Cursor += 4;

		//Ambient
		std::getline(StringStream, TextureName);
		if (TextureName != "None") {
			if (!m_AssetManagerReference->RequestTexture2D(TextureName, Texture_ptr)) {
				//Print error message! Get it from asset manager or something?
			}
			m_Ambient = Texture_ptr;
			Cursor += TextureName.size();
		}
		else
			Cursor += 4;

		//Specular
		std::getline(StringStream, TextureName);
		if (TextureName != "None") {
			if (!m_AssetManagerReference->RequestTexture2D(TextureName, Texture_ptr)) {
				//Print error message! Get it from asset manager or something?
			}
			m_Specular = Texture_ptr;
			Cursor += TextureName.size();
		}
		else
			Cursor += 4;

		std::memcpy(&m_AmbientStrength, &buffer[0] + Cursor, 4);
		Cursor += 4;

		std::memcpy(&m_SpecularStrength, &buffer[0] + Cursor, 4);
		Cursor += 4;

		std::memcpy(&m_SpecularShininess, &buffer[0] + Cursor, 4);
		Cursor += 4;

		return true;
	}

public:
	Texture2D* m_Diffuse	{ nullptr };
	Texture2D* m_Ambient	{ nullptr };
	Texture2D* m_Specular	{ nullptr };

	float m_AmbientStrength = 1.0f;

	float m_SpecularStrength = 0.5f;
	int32 m_SpecularShininess = 32;

private:
	Asset* m_Asset							{ nullptr };
	AssetManager* m_AssetManagerReference	{ nullptr };
};