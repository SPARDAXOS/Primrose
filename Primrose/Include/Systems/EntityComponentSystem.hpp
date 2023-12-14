#pragma once
#include "Components/SpriteRendererComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Components/SkeletalMeshComponent.hpp"

#include "Structures/MemoryBlocksBucket.h"

class Core;
class GameObject;

constexpr uint64 INVALID_OBJECT_ID = 0;
constexpr int8 FAILED_COMPONENT_SEARCH = -1;

class EntityComponentSystem final {
public:
	EntityComponentSystem() = delete;
	EntityComponentSystem(Core& core) noexcept;
	~EntityComponentSystem();

	EntityComponentSystem(const EntityComponentSystem& other) = delete;
	EntityComponentSystem& operator=(const EntityComponentSystem& other) = delete;

	EntityComponentSystem(EntityComponentSystem&& other) = delete;
	EntityComponentSystem& operator=(EntityComponentSystem&& other) = delete;


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
	SkeletalMesh*	  AddComponent<SkeletalMesh>(uint64 objectID);
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

		const uint64 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return;

		m_SpriteRenderers.erase(std::begin(m_SpriteRenderers) + TargetIndex);
	}
	template<>
	void RemoveComponent<SkeletalMesh>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const uint64 TargetIndex = FindSkeletalMesh(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return;

		m_SkeletalMeshes.erase(std::begin(m_SkeletalMeshes) + TargetIndex);
	}
	template<>
	void RemoveComponent<Camera>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const uint64 TargetIndex = FindCamera(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
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

		const uint64 TargetIndex = FindPointLight(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return;

		m_PointLights.erase(std::begin(m_PointLights) + TargetIndex);
	}
	template<>
	void RemoveComponent<SpotLight>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const uint64 TargetIndex = FindSpotLight(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return;

		m_SpotLights.erase(std::begin(m_SpotLights) + TargetIndex);
	}


	template<typename T>
	T* GetComponent(uint64 objectID);
	template<>
	SpriteRenderer* GetComponent<SpriteRenderer>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const uint64 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return nullptr;

		return &m_SpriteRenderers.at(TargetIndex);
	}
	template<>
	SkeletalMesh* GetComponent<SkeletalMesh>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const uint64 TargetIndex = FindSkeletalMesh(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return nullptr;

		return m_SkeletalMeshes.at(TargetIndex);
	}
	template<>
	Camera* GetComponent<Camera>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const uint64 TargetIndex = FindCamera(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return nullptr;

		return &m_Cameras.at(TargetIndex);
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

		const uint64 TargetIndex = FindPointLight(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return nullptr;

		return m_PointLights.at(TargetIndex);
	}
	template<>
	SpotLight* GetComponent<SpotLight>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const uint64 TargetIndex = FindSpotLight(objectID);
		if (TargetIndex == FAILED_COMPONENT_SEARCH)
			return nullptr;

		return m_SpotLights.at(TargetIndex);
	}


public:
	//TODO: LATEST NOTE: Yes, delete all these and simply return the vector.
	template<typename T>
	std::vector<T> GetAllComponentsOfType() const noexcept;

	template<>
	std::vector<SpriteRenderer> GetAllComponentsOfType<SpriteRenderer>() const noexcept{
		return m_SpriteRenderers;
	}
	template<>
	std::vector<Camera> GetAllComponentsOfType<Camera>() const noexcept {
		return m_Cameras;
	}



public: //NOTE: These seem to serve the same purpose as GetAllComponentsOfType which has a better name...
	inline std::vector<GameObject*> GetGameObjects() const noexcept { return m_GameObjects; }
	inline std::vector<SpriteRenderer> GetSpriteRenderers() const noexcept { return m_SpriteRenderers; }
	inline std::vector<SkeletalMesh*> GetSkeletalMeshes() const noexcept { return m_SkeletalMeshes; }
	inline std::vector<PointLight*> GetPointLights() const noexcept { return m_PointLights; }
	inline std::vector<SpotLight*> GetSpotLights() const noexcept { return m_SpotLights; }



	//TODO: maybe rework this to use lists? simply passes the vector?. LATEST NOTE: Yes, delete all these and simply return the vector.
	template<typename T>
	T* GetComponentForUpdate();
	template<>
	SpriteRenderer* GetComponentForUpdate<SpriteRenderer>() {
		SpriteRenderer* TargetComponent = &m_SpriteRenderers.at(m_SpriteRenderersUpdateIndex);
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
	int32 FindSkeletalMesh(uint64 objectID) const noexcept;
	int32 FindCamera(uint64 objectID) const noexcept;
	int32 FindPointLight(uint64 objectID) const noexcept;
	int32 FindSpotLight(uint64 objectID) const noexcept;

private:
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; }

private:
	uint32  m_SpriteRenderersUpdateIndex = 0; //?? Remove probably


private:
	std::vector<GameObject*> m_GameObjects;

	//NEW 
	//Each pointer points to the start of a bucket of X predefined constexpr size
	std::vector<SpriteRenderer*> m_SpriteRenderersBuckets;
	//Need list of places std::vector<bool> for vacantSpotsOnMemoryBlock
	std::vector<std::vector<bool>> m_SpriteRenderersBucketsLayout; //Questionable doesnt seem elegant


	std::vector<SpriteRenderer> m_SpriteRenderers;
	std::vector<SkeletalMesh*> m_SkeletalMeshes; 
	std::vector<Camera> m_Cameras;

	DirectionalLight* m_MainDirectionalLight;
	std::vector<PointLight*> m_PointLights;
	std::vector<SpotLight*> m_SpotLights;

private:
	std::string m_LastExitMessage;

	Core* m_Core	{ nullptr };

	//Created or loaded later on
	GameObject* m_MainScene; 
	GameObject* m_ViewportCameraGO;
	Camera* m_ViewportCamera;

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
	template<>
	inline uint32 GetComponentID<SkeletalMesh>() noexcept { return SKELETAL_MESH_COMPONENT_ID; }

	//TODO: Add Custom Component which is basically a component with no code that can be customized.
}