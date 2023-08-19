#include "Systems/Physics.hpp"
#include "Systems/Core.hpp"

Physics::Physics(Core& core) noexcept 
	:	m_CoreReference(&core)
{
	m_ViewportCameraRef = &m_CoreReference->GetECS()->GetViewportCamera();
}

//CheckViewportSelection()


Vector3f Physics::ScreenToWorld(Vector2f position, float z) {

	const auto InverseView = glm::inverse(m_ViewportCameraRef->GetProjectionMatrix() * m_ViewportCameraRef->GetViewMatrix());

	glm::vec4 ViewPosition = InverseView * glm::vec4(position.m_X, position.m_Y, z, 1.0f);
	ViewPosition.x /= ViewPosition.w;
	ViewPosition.y /= ViewPosition.w;
	ViewPosition.z /= ViewPosition.w;
	Vector3f Results(ViewPosition.x, ViewPosition.y, ViewPosition.z);

	//Projection then view to bring back to world?

	//mat4 inversePrjMat = inverse( prjMat );
	//vec4 viewPosH = inversePrjMat * vec4(ndc_x, ndc_y, 2.0 * depth - 1.0, 1.0)
	//vec3 viewPos = viewPos.xyz / viewPos.w;

	return Results;
}
HitResults Physics::Raycast(Vector3f start, Vector3f direction, float extent) {

	HitResults Results;





	return Results;
}