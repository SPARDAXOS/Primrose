#include "Components/SpotLightComponent.hpp"
#include "GameObject.hpp"


SpotLight::SpotLight(GameObject& owner, uint64 ownerID)
	: LightComponentBase(owner, ownerID)
{

}

Vector4f SpotLight::GetDirection() const noexcept {

	const Transform* LightTransform = &m_Owner->GetTransform();
	Vector4f LightDirection;
	LightDirection.m_X = static_cast<float>(cos((LightTransform->m_Rotation.m_X / 180) * PI()));
	LightDirection.m_Y = static_cast<float>(sin((LightTransform->m_Rotation.m_Y / 180) * PI()));
	LightDirection.m_Z = static_cast<float>(tan((LightTransform->m_Rotation.m_Z / 180) * PI()));
	LightDirection.m_W = 0.0f; //Cause its position doesnt matter.

	return LightDirection;
}
float SpotLight::GetInnerAngleCosine() const noexcept {
	return static_cast<float>(cos((m_InnerCutoffAngle * (PI() / 180.0f))));
}
float SpotLight::GetOuterAngleCosine() const noexcept {
	return static_cast<float>(cos((m_OuterCutoffAngle * (PI() / 180.0f))));
}