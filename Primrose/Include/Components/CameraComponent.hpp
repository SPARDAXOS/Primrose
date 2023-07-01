#pragma once
#include "Component.hpp"
#include "Transform.hpp"
#include "Math.hpp"
#include "Utility.hpp"

class GameObject;

class Camera final : public ComponentBase {
public:
	enum class ProjectionType { ORTHOGRAPHIC, PERSPECTIVE };

public:
	Camera() = delete;
	Camera(GameObject& owner, uint64 ownerID);
	~Camera() = default;

	//For now it is not possbile to move or copy components - Copy only it should be? idk think about it
	Camera(const Camera& other) = delete;
	Camera& operator=(const Camera& other) = delete;

	Camera(Camera&& other) = delete;
	Camera& operator=(Camera&& other) = delete;

public:
	inline glm::mat4 GetViewMatrix() const noexcept { return m_ViewMatrix; };
	inline glm::mat4 GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; };

	inline ProjectionType GetProjectionType() const noexcept { return m_ProjectionType; };
	inline float GetWidth() const noexcept { return m_Width; };
	inline float GetHeight() const noexcept { return m_Height; };
	inline float GetFOV() const noexcept { return m_FOV; };
	inline float GetAspectRatio() const noexcept { return m_AspectRatio; };
	inline float GetNearClipPlane() const noexcept { return m_NearClipPlane; };
	inline float GetFarClipPlane() const noexcept { return m_FarClipPlane; };

	inline void SetProjectionType(ProjectionType type) {
		m_ProjectionType = type;
		UpdateProjectionMatrix();
	};
	inline void SetWidth(float width) {
		m_Width = width;
		if (m_ProjectionType == ProjectionType::ORTHOGRAPHIC)
			UpdateProjectionMatrix();
	};
	inline void SetHeight(float height) {
		m_Height = height;
		if (m_ProjectionType == ProjectionType::ORTHOGRAPHIC)
			UpdateProjectionMatrix();
	};
	inline void SetFOV(float fov) {
		//TODO: Set some caps on this
		m_FOV = fov;
		if (m_ProjectionType == ProjectionType::PERSPECTIVE)
			UpdateProjectionMatrix();
	};
	inline void SetAspectRatio(float ratio) {
		m_AspectRatio = ratio;
		if (m_ProjectionType == ProjectionType::PERSPECTIVE)
			UpdateProjectionMatrix();
	};
	inline void SetNearClipPlane(float distance) {
		m_NearClipPlane = distance;
		UpdateProjectionMatrix();
	};
	inline void SetFarClipPlane(float distance) {
		m_FarClipPlane = distance;
		UpdateProjectionMatrix();
	};


private:
	inline void UpdateProjectionMatrix() {
		if (m_ProjectionType == ProjectionType::PERSPECTIVE)
			m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClipPlane, m_FarClipPlane);
		else if (m_ProjectionType == ProjectionType::ORTHOGRAPHIC)
			m_ProjectionMatrix = glm::ortho(0.0f, m_Width, 0.0f, m_Height, m_NearClipPlane, m_FarClipPlane);
	}

private:
	ProjectionType m_ProjectionType = ProjectionType::PERSPECTIVE;
	float m_Width = 800.0f;
	float m_Height = 600.0f;
	float m_FOV = 45.0f;
	float m_AspectRatio = m_Width / m_Height;
	float m_NearClipPlane = 0.1f;
	float m_FarClipPlane = 100.0f;

private:
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::vec3 m_Right;
	glm::vec3 m_Up;
};