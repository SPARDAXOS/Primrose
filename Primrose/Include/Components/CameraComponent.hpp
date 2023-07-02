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
	glm::mat4 GetViewMatrix() noexcept { 
		UpdateViewMatrix();
		return m_ViewMatrix; 
	};
	glm::mat4 GetProjectionMatrix() noexcept { 
		UpdateProjectionMatrix();
		return m_ProjectionMatrix; 
	};

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

	//Make axis param instead?
	void MoveX(float amount);
	void MoveY(float amount) noexcept;
	void MoveVertical(float amount) noexcept;
	void RotateX(float amount) noexcept;
	void RotateY(float amount) noexcept;


	void UpdateVectors() noexcept;


private:
	void UpdateProjectionMatrix();
	void UpdateViewMatrix();

private:
	ProjectionType m_ProjectionType = ProjectionType::PERSPECTIVE;
	float m_Width = 800.0f;
	float m_Height = 600.0f;
	float m_FOV = 45.0f;
	float m_AspectRatio = m_Width / m_Height;
	float m_NearClipPlane = 0.1f;
	float m_FarClipPlane = 100.0f;

private:
	float m_Yaw = -90.0f;
	float m_Roll = 0.0f;
	float m_Pitch = 0.0f;

private:
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::vec3 m_Right;
	glm::vec3 m_Up;
	glm::vec3 m_Forward;
};