#include "Components/DirectionalLightComponent.hpp"
#include "GameObject.hpp"



DirectionalLight::DirectionalLight(GameObject& owner, int64 ownerID)
	: LightComponentBase(owner, ownerID)
{

}

Vector4f DirectionalLight::GetDirection() const noexcept {

	const Transform* LightTransform = &m_Owner->GetTransform();
	Vector4f LightDirection;
	LightDirection.m_X = static_cast<float>(cos((LightTransform->m_Rotation.m_X / 180) * PI()));
	LightDirection.m_Y = static_cast<float>(sin((LightTransform->m_Rotation.m_Y / 180) * PI()));
	LightDirection.m_Z = static_cast<float>(tan((LightTransform->m_Rotation.m_Z / 180) * PI()));
	LightDirection.m_W = 0.0f; //Cause its position doesnt matter.

	return LightDirection;
}