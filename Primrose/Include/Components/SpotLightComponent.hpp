#pragma once
#include "LightComponentBase.hpp"

class SpotLight final : public LightComponentBase {
public:
	SpotLight() = delete;
	SpotLight(GameObject& owner, int64 ownerID);
	~SpotLight() = default;

	//For now it is not possbile to move or copy components - Copy only it should be? idk think about it
	SpotLight(const SpotLight& other) = delete;
	SpotLight& operator=(const SpotLight& other) = delete;

	SpotLight(SpotLight&& other) = delete;
	SpotLight& operator=(SpotLight&& other) = delete;

public:
	Vector4f GetDirection() const noexcept;
	float GetInnerAngleCosine() const noexcept;
	float GetOuterAngleCosine() const noexcept;

public:
	float m_InnerCutoffAngle = 45.0f;
	float m_OuterCutoffAngle = 45.0f;
	float m_Attenuation = 0.09f;
	float m_SourceRadius = 0.032f;
};
