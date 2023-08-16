#pragma once
#include "Components/SpriteRendererComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"

#include <vector>

class Core;
class GameObject;

constexpr int INVALID_OBJECT_ID = -1;

class EntityComponentSystem final {
public:
	EntityComponentSystem() = delete;
	EntityComponentSystem(Core& core) noexcept;
	~EntityComponentSystem();

	EntityComponentSystem(const EntityComponentSystem&) = delete;
	EntityComponentSystem& operator=(const EntityComponentSystem&) = delete;

	EntityComponentSystem(EntityComponentSystem&&) = delete;
	EntityComponentSystem& operator=(EntityComponentSystem&&) = delete;


public:
	GameObject& CreateGameObject();
	GameObject& CreateGameObject(const std::string& name);
	GameObject& Instantiate(const GameObject& object);

	void DestroyGameObject(uint64 objectID);

public:
	[[nodiscard]] bool Update();
	

public:
	template<typename T>
	T* AddComponent(uint64 objectID);
	template<>
	SpriteRenderer*   AddComponent<SpriteRenderer>(uint64 objectID);
	template<>
	Camera*			  AddComponent<Camera>(uint64 objectID);
	template<>
	DirectionalLight* AddComponent<DirectionalLight>(uint64 objectID);
	template<>
	PointLight*		  AddComponent<PointLight>(uint64 objectID);
	template<>
	SpotLight*		  AddComponent<SpotLight>(uint64 objectID);


	template<typename T>
	void RemoveComponent(uint64 objectID);
	template<>
	void RemoveComponent<SpriteRenderer>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const int32 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == INVALID_OBJECT_ID) 
			return;

		m_SpriteRenderers.erase(std::begin(m_SpriteRenderers) + TargetIndex);
	}
	template<>
	void RemoveComponent<Camera>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const int32 TargetIndex = FindCamera(objectID);
		if (TargetIndex == INVALID_OBJECT_ID) 
			return;

		m_Cameras.erase(std::begin(m_Cameras) + TargetIndex);
	}
	template<>
	void RemoveComponent<DirectionalLight>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		//GameObject->RemoveComponent() checks if it has the component then it calls this so you can assume that it exists.
		delete m_MainDirectionalLight;
		m_MainDirectionalLight = nullptr;
	}
	template<>
	void RemoveComponent<PointLight>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const int32 TargetIndex = FindPointLight(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return;

		m_PointLights.erase(std::begin(m_PointLights) + TargetIndex);
	}
	template<>
	void RemoveComponent<SpotLight>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const int32 TargetIndex = FindSpotLight(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return;

		m_SpotLights.erase(std::begin(m_SpotLights) + TargetIndex);
	}


	template<typename T>
	T* GetComponent(uint64 objectID);
	template<>
	SpriteRenderer* GetComponent<SpriteRenderer>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const int32 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return nullptr;

		return m_SpriteRenderers.at(TargetIndex);
	}
	template<>
	Camera* GetComponent<Camera>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const int32 TargetIndex = FindCamera(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return nullptr;

		return m_Cameras.at(TargetIndex);
	}
	template<>
	DirectionalLight* GetComponent<DirectionalLight>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		//GameObject->GetComponent() checks if it has the component then it calls this so you can assume that it exists.
		return m_MainDirectionalLight;
	}
	template<>
	PointLight* GetComponent<PointLight>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const int32 TargetIndex = FindPointLight(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return nullptr;

		return m_PointLights.at(TargetIndex);
	}
	template<>
	SpotLight* GetComponent<SpotLight>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const int32 TargetIndex = FindSpotLight(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return nullptr;

		return m_SpotLights.at(TargetIndex);
	}


public:
	template<typename T>
	uint32 GetComponentsAmount() const noexcept;
	template<>
	uint32 GetComponentsAmount<SpriteRenderer>() const noexcept{
		return static_cast<uint32>(m_SpriteRenderers.size());
	}
	template<>
	uint32 GetComponentsAmount<Camera>() const noexcept {
		return static_cast<uint32>(m_Cameras.size());
	}



public: //TODO: maybe rework this to use lists? simply passes the vector?

	inline std::vector<GameObject*> GetGameObjects() const noexcept { return m_GameObjects; }
	inline std::vector<PointLight*> GetPointLights() const noexcept { return m_PointLights; }
	inline std::vector<SpotLight*> GetSpotLights() const noexcept { return m_SpotLights; }

	template<typename T>
	T* GetComponentForUpdate();
	template<>
	SpriteRenderer* GetComponentForUpdate<SpriteRenderer>() {
		SpriteRenderer* TargetComponent = m_SpriteRenderers.at(m_SpriteRenderersUpdateIndex);
		m_SpriteRenderersUpdateIndex++;
		if (m_SpriteRenderersUpdateIndex == m_SpriteRenderers.size())
			m_SpriteRenderersUpdateIndex = 0;
		return TargetComponent;
	}




public:
	inline std::string GetLastExitMessage() const noexcept { return m_LastExitMessage; }


	inline GameObject& GetCurrentScene() const noexcept { return *m_MainScene; }
	inline Camera& GetViewportCamera() const noexcept { return *m_ViewportCamera; }
	inline DirectionalLight* GetMainDirectionalLight() const noexcept { return m_MainDirectionalLight; }


	GameObject* FindGameObject(uint64 ObjectID) const noexcept;
	bool IsReserved(uint64 objectID) const noexcept;

private:
	void CalculateTransformations(GameObject& object);

private:
	int32 FindSpriteRenderer(uint64 objectID) const noexcept;
	int32 FindCamera(uint64 objectID) const noexcept;
	int32 FindPointLight(uint64 objectID) const noexcept;
	int32 FindSpotLight(uint64 objectID) const noexcept;

private:
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; }

private:
	uint32  m_SpriteRenderersUpdateIndex = 0;


private:
	std::vector<GameObject*> m_GameObjects;
	std::vector<SpriteRenderer*> m_SpriteRenderers;
	std::vector<Camera*> m_Cameras;
	
	std::vector<PointLight*> m_PointLights;
	std::vector<SpotLight*> m_SpotLights;

private:
	std::string m_LastExitMessage;

	Core* m_CoreReference	{ nullptr };

	//Created or loaded later on
	GameObject* m_MainScene; 
	GameObject* m_ViewportCameraGO;
	Camera* m_ViewportCamera;
	DirectionalLight* m_MainDirectionalLight;

	uint64 m_CurrentObjectIDIndex = 1;
};



namespace Components {
	//You add new components template specializations here
	template<typename T>
	inline uint32 GetComponentID() noexcept;



	template<>
	inline uint32 GetComponentID<SpriteRenderer>() noexcept { return SPRITE_RENDERER_COMPONENT_ID; }
	template<>
	inline uint32 GetComponentID<Camera>() noexcept { return CAMERA_COMPONENT_ID; }
	template<>
	inline uint32 GetComponentID<DirectionalLight>() noexcept { return DIRECTIONAL_LIGHT_COMPONENT_ID; }
	template<>
	inline uint32 GetComponentID<PointLight>() noexcept { return POINT_LIGHT_COMPONENT_ID; }
	template<>
	inline uint32 GetComponentID<SpotLight>() noexcept { return SPOT_LIGHT_COMPONENT_ID; }

	//TODO: Add Custom Component which is basically a component with no code that can be customized.
}