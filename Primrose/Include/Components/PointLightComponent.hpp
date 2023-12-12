#pragma once
#include "LightComponentBase.hpp"

class PointLight final : public LightComponentBase {
public:
	PointLight() = delete;
	PointLight(GameObject& owner, int64 ownerID);
	~PointLight() = default;

	//For now it is not possbile to move or copy components - Copy only it should be? idk think about it
	PointLight(const PointLight& other) = delete;
	PointLight& operator=(const PointLight& other) = delete;

	PointLight(PointLight&& other) = delete;
	PointLight& operator=(PointLight&& other) = delete;

public:
	float m_Attenuation = 0.09f;
	float m_SourceRadius = 0.032f;


};

