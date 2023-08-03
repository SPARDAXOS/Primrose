#include "Utility.hpp"
#include "Texture2D.hpp"


class Material {
public:

	Material();
	~Material();

	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;

	Material(Material&&) = delete;
	Material& operator=(Material&&) = delete;




private:
	//Actual textures*
	//Diffuse -2D it will take the sprite instead of this -3D it will take this!
	//Ambient
	//Specualr
	//Default error texture!

	Texture2D* m_Diffuse	{ nullptr };
	Texture2D* m_Ambient	{ nullptr };
	Texture2D* m_Specular	{ nullptr };

	uint32 m_SpecularStrength;
	float m_SpecularShininess;

	float m_AmbientStrength;

};