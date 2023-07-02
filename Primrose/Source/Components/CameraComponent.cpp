#include "Components/SpriteRendererComponent.hpp"
#include "GameObject.hpp"

Camera::Camera(GameObject& owner, uint64 ownerID)
	: ComponentBase(owner, ownerID)
{
	//Construct matrices - At least the projection
	Transform* TargetTransform = &m_Owner->GetTransform();
	TargetTransform->m_Position.m_Z = 3.0f;
	TargetTransform->m_Rotation.m_Y = -90.0;


}

void Camera::UpdateProjectionMatrix() {
	if (m_ProjectionType == ProjectionType::PERSPECTIVE)
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClipPlane, m_FarClipPlane);
	else if (m_ProjectionType == ProjectionType::ORTHOGRAPHIC)
		m_ProjectionMatrix = glm::ortho(0.0f, m_Width, 0.0f, m_Height, m_NearClipPlane, m_FarClipPlane);
}
void Camera::UpdateViewMatrix() {
	Transform* TargetTransform = &m_Owner->GetTransform();
	const glm::vec3 Position
		= glm::vec3(TargetTransform->m_Position.m_X, TargetTransform->m_Position.m_Y, TargetTransform->m_Position.m_Z);

	const glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	const glm::vec3 ReverseDirection = glm::normalize(Position - CameraTarget);
	const glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_Right = glm::normalize(glm::cross(Up, ReverseDirection));

	m_Up = glm::cross(ReverseDirection, m_Right);

	//Todo: update, start and maybe awake for gameobjects.
	//Todo: add error handling for ecs
	//Todo: add enabled check for update and renderer. Component and gameobject.
	//Todo: redo projection and view calculations in get functions


	//TargetTransform->m_Rotation.m_X = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	//TargetTransform->m_Rotation.m_Y = sin(glm::radians(m_Pitch));
	//TargetTransform->m_Rotation.m_Z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));


	//The above shit might as well be useless i think
	const float Pitch = cos(glm::radians(TargetTransform->m_Rotation.m_Y)) * cos(glm::radians(TargetTransform->m_Rotation.m_X));
	const float Yaw = sin(glm::radians(TargetTransform->m_Rotation.m_X));
	const float Roll = sin(glm::radians(TargetTransform->m_Rotation.m_Y)) * cos(glm::radians(TargetTransform->m_Rotation.m_X));

	glm::vec3 Vector{Pitch, Yaw, Roll};
	m_Forward = glm::normalize(Vector);

	m_ViewMatrix = glm::lookAt(Position, Position + m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)); //CameraTarget
}

//void Camera::UpdateRightVector() noexcept {
//
//}

void Camera::MoveX(float amount) noexcept {
	//Note: I removed the normalization for now since it made the movement too fast for some reason.
	Transform* OwnerTransform = &m_Owner->GetTransform();
	OwnerTransform->m_Position += glm::cross(m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)) * amount;

	if (OwnerTransform->m_Position.m_X > 89.0f)
		OwnerTransform->m_Position.m_X = 89.0f;
	if (OwnerTransform->m_Position.m_X < -89.0f)
		OwnerTransform->m_Position.m_X = -89.0f;
}
void Camera::MoveY(float amount) noexcept {
	Transform* OwnerTransform = &m_Owner->GetTransform();
	Vector3f Results = Vector3f(m_Forward) * amount;
	//Results.Normalize(); //TODO: CHECK IF THE CALCULATION FOR THIS IS CORRECT!
	OwnerTransform->m_Position += Results;
}

