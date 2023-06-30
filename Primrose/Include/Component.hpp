#pragma once
#include "Utility.hpp"
#include "Math.hpp"
#include "GraphicsResources.hpp"
#include "TextureStorage.hpp"

constexpr uint32 INVALID_ID = 0b00000000000000000000000000000000;
constexpr uint32 SPRITE_COMPONENT_ID = 0b00000000000000000000000000000001;
constexpr uint32 CAMERA_COMPONENT_ID = 0b00000000000000000000000000000010;

//Move somewhere else or get rid of it
constexpr uint32 MAIN_SCENE_OBJECT_ID = 0b00000000000000000000000000000001;

class GameObject;

class ComponentBase {
public:
	ComponentBase() = delete;
	ComponentBase(GameObject& owner, uint64 ownerID) noexcept 
		: m_Owner(&owner), m_OwnerID(ownerID)
	{
	}


public:
	inline void SetEnabled(bool state) noexcept { m_Enabled = state; };
	inline bool GetEnabled() const noexcept { return m_Enabled; };
	inline uint64 GetOwnerID() const noexcept { return m_OwnerID; };
	inline GameObject* GetOwner() const noexcept { return m_Owner; };


protected:
	bool m_Enabled{ true };
	uint64 m_OwnerID;
	GameObject* m_Owner;
};

class Transform final {
public:
	explicit Transform() = default;
	Transform(Vector3f position, Vector3f rotation, Vector3f scale) noexcept
		: m_Position(position), m_Rotation(rotation), m_Scale(scale)
	{
	}
	Transform(Vector3f position, Vector3f rotation) noexcept
		: m_Position(position), m_Rotation(rotation), m_Scale(Vector3f(1.0f))
	{
	}
	Transform(Vector3f position) noexcept
		: m_Position(position), m_Rotation(Vector3f(0.0f)), m_Scale(Vector3f(1.0f))
	{
	}

	~Transform() = default;

	Transform(const Transform& other) noexcept {
		*this = other;
	}
	Transform& operator=(const Transform& other) noexcept {
		if (this == &other) {
			return *this;
		}
		else {
			this->m_Position = other.m_Position;
			this->m_Rotation = other.m_Rotation;
			this->m_Scale = other.m_Scale;
			return *this;
		}
	}

	Transform(Transform&& other) noexcept {
		*this = std::move(other);
	}
	Transform& operator=(Transform&& other) noexcept {
		if (this == &other) {
			return *this;
		}
		else {
			this->m_Position = std::move(other.m_Position);
			this->m_Rotation = std::move(other.m_Rotation);
			this->m_Scale = std::move(other.m_Scale);
			return *this;
		}
	}

public:
	[[nodiscard]] inline glm::mat4 GetMatrix() { UpdateMatrix(); return m_Matrix; };

private:
	void UpdateMatrix() {
		m_Matrix = glm::mat4(1.0f);

		m_Matrix = glm::translate(m_Matrix, glm::vec3(m_Position.m_X, m_Position.m_Y, m_Position.m_Z));

		m_Matrix = glm::scale(m_Matrix, glm::vec3(m_Scale.m_X, m_Scale.m_Y, m_Scale.m_Z));

		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_X), glm::vec3(1.0f, 0.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, glm::radians(m_Rotation.m_Z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

public:
	Vector3f m_Position{ 0.0f };
	Vector3f m_Rotation{ 0.0f };
	Vector3f m_Scale{ 1.0f };

private:
	glm::mat4 m_Matrix;
};

class SpriteRenderer final : public ComponentBase {
public:
	SpriteRenderer() = delete;
	SpriteRenderer(GameObject& owner, uint64 ownerID) noexcept 
		:	ComponentBase(owner, ownerID)
	{
		//TODO: Clean this up. This looks messy!
		m_VAO = new VAO;
		m_VAO->Bind();

		m_VBO = new VBO(m_Primitive.m_Data, sizeof(m_Primitive.m_Data));
		m_EBO = new EBO(m_Primitive.m_Indices, sizeof(m_Primitive.m_Indices), sizeof(m_Primitive.m_Indices) / sizeof(GLuint));

		m_VBO->Bind();
		m_EBO->Bind();

		m_VAO->Unbind();
		m_VBO->Unbind();
		m_EBO->Unbind();
	}
	~SpriteRenderer() {
		delete m_VAO;
		delete m_VBO;
		delete m_EBO;
	}


	//For now it is not possbile to move or copy components
	SpriteRenderer(const SpriteRenderer& other) = delete;
	SpriteRenderer& operator=(const SpriteRenderer& other) = delete;

	SpriteRenderer(SpriteRenderer&& other) = delete;
	SpriteRenderer& operator=(SpriteRenderer&& other) = delete;

public:
	inline void SetSprite(Texture2D* sprite) noexcept { m_Sprite = sprite; }
	inline void SetTint(Color color) noexcept { m_Tint = color; }
	inline void SetAddressingMode(AddressingMode s, AddressingMode t) noexcept { m_AddressingModeS = s; m_AddressingModeT = t; }
	inline void SetFilteringMode(FilteringMode min, FilteringMode mag) noexcept { m_FilteringModeMin = min; m_FilteringModeMag = mag; }


	inline Texture2D* GetSprite() const noexcept { return m_Sprite; }
	inline Color GetTint() const noexcept { return m_Tint; }
	inline AddressingMode GetAddressingModeS() const noexcept { return m_AddressingModeS; }
	inline AddressingMode GetAddressingModeT() const noexcept { return m_AddressingModeT; }
	inline FilteringMode GetFilteringModeMin() const noexcept { return m_FilteringModeMin; }
	inline FilteringMode GetFilteringModeMag() const noexcept { return m_FilteringModeMag; }
	
	inline VAO* GetVAO() const noexcept { return m_VAO; }
	inline EBO* GetEBO() const noexcept { return m_EBO; }


private:
	Texture2D* m_Sprite = nullptr;
	Color m_Tint = Colors::White;

private:
	AddressingMode m_AddressingModeS = AddressingMode::REPEAT;
	AddressingMode m_AddressingModeT = AddressingMode::REPEAT;
	FilteringMode m_FilteringModeMin = FilteringMode::LINEAR_MIPMAP_LINEAR;
	FilteringMode m_FilteringModeMag = FilteringMode::LINEAR;

private:
	Cube m_Primitive; //Change to square for 2D
	VAO* m_VAO = nullptr;
	VBO* m_VBO = nullptr;
	EBO* m_EBO = nullptr;
};

class Camera final : public ComponentBase {
public:
	enum class ProjectionType { ORTHOGRAPHIC, PERSPECTIVE };

public:
	Camera() = delete;
	Camera(GameObject& owner, uint64 ownerID)
		: ComponentBase(owner, ownerID)
	{
		//Construct matrices - At least the projection

		UpdateProjectionMatrix();

		ViewMatrix = glm::mat4(1.0f);
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));

	}
	~Camera() = default;

	//For now it is not possbile to move or copy components
	Camera(const Camera& other) = delete;
	Camera& operator=(const Camera& other) = delete;

	Camera(Camera&& other) = delete;
	Camera& operator=(Camera&& other) = delete;

public:
	inline glm::mat4 GetViewMatrix() const noexcept { return ViewMatrix; };
	inline glm::mat4 GetProjectionMatrix() const noexcept { return ProjectionMatrix; };

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
			ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClipPlane, m_FarClipPlane);
		else if (m_ProjectionType == ProjectionType::ORTHOGRAPHIC)
			ProjectionMatrix = glm::ortho(0.0f, m_Width, 0.0f, m_Height, m_NearClipPlane, m_FarClipPlane);
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
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
};


namespace Components {
	//You add new components template specializations here
	template<typename T>
	inline uint32 GetComponentID() noexcept;
	template<>
	inline uint32 GetComponentID<SpriteRenderer>() noexcept { return SPRITE_COMPONENT_ID; }
	template<>
	inline uint32 GetComponentID<Camera>() noexcept { return CAMERA_COMPONENT_ID; }

	//TODO: Add Custom Component which is basically a component with no code that can be customized.
}