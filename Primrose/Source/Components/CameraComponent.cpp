#include "Components/SpriteRendererComponent.hpp"
#include "GameObject.hpp"

Camera::Camera(GameObject& owner, uint64 ownerID)
	: ComponentBase(owner, ownerID)
{
	//Construct matrices - At least the projection

	UpdateProjectionMatrix();

	Transform* TargetTransform = &owner.GetTransform();
	TargetTransform->m_Position.m_Z = 3.0f;

	glm::vec3 Position = glm::vec3(TargetTransform->m_Position.m_X, TargetTransform->m_Position.m_Y, TargetTransform->m_Position.m_Z);

	glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraDirection = glm::normalize(Position - CameraTarget);

	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_Right = glm::normalize(glm::cross(Up, CameraDirection));
	m_Up = glm::cross(CameraDirection, m_Right);

	m_ViewMatrix = glm::lookAt(Position, CameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));


}