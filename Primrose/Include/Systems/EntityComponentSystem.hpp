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
	SpriteRenderer* AddComponent<SpriteRenderer>(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, SpriteRenderer>); //Makes more sense for the custom components
		GameObject* ptr = FindGameObject(objectID);
		if (ptr == nullptr)
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical


		SpriteRenderer* NewSpriteRenderer = new SpriteRenderer(*ptr, objectID); //Add index in 
		m_SpriteRenderers.push_back(NewSpriteRenderer);
		return NewSpriteRenderer;
	}
	template<>
	Camera* AddComponent<Camera>(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, Camera>); //Makes more sense for the custom components
		GameObject* ptr = FindGameObject(objectID);
		if (ptr == nullptr)
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical


		Camera* NewCamera = new Camera(*ptr, objectID); //Add index in 
		m_Cameras.push_back(NewCamera);
		return NewCamera;
	}
	template<>
	DirectionalLight* AddComponent<DirectionalLight>(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, DirectionalLight>); //Makes more sense for the custom components
		GameObject* ptr = FindGameObject(objectID);
		if (ptr == nullptr)
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical


		DirectionalLight* NewDirectionalLight = new DirectionalLight(*ptr, objectID); //Add index in ? //Latest note. There are a lot of search optimizations that could be done!
		m_DirectionalLights.push_back(NewDirectionalLight);
		return NewDirectionalLight;
	}
	template<>
	PointLight* AddComponent<PointLight>(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, PointLight>); //Makes more sense for the custom components
		GameObject* ptr = FindGameObject(objectID);
		if (ptr == nullptr)
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical

		PointLight* NewPointLight = new PointLight(*ptr, objectID); //Add index in ? //Latest note. There are a lot of search optimizations that could be done!
		m_PointLights.push_back(NewPointLight);
		return NewPointLight;
	}
	template<>
	SpotLight* AddComponent<SpotLight>(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, SpotLight>); //Makes more sense for the custom components
		GameObject* ptr = FindGameObject(objectID);
		if (ptr == nullptr)
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical

		SpotLight* NewSpotLight = new SpotLight(*ptr, objectID);
		m_SpotLights.push_back(NewSpotLight);
		return NewSpotLight;
	}


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

		const int32 TargetIndex = FindDirectionalLight(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return;

		m_DirectionalLights.erase(std::begin(m_DirectionalLights) + TargetIndex);
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

		const int32 TargetIndex = FindDirectionalLight(objectID);
		if (TargetIndex == INVALID_OBJECT_ID)
			return nullptr;

		return m_DirectionalLights.at(TargetIndex);
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

	//Will all be deleted when multiple light casters are supported
	GameObject* GetDirecitonalLightTEST() const noexcept;
	GameObject* GetPointLightTEST() const noexcept;
	GameObject* GetSpotLightTEST() const noexcept;


	GameObject* FindGameObject(uint64 ObjectID) const noexcept;

	bool IsReserved(uint64 objectID) const noexcept;

private:
	void CalculateTransformations(GameObject& object);

private:
	int32 FindSpriteRenderer(uint64 objectID) const noexcept;
	int32 FindCamera(uint64 objectID) const noexcept;
	int32 FindDirectionalLight(uint64 objectID) const noexcept; //It will only be 1
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
	std::vector<DirectionalLight*> m_DirectionalLights; //????? It will only be allowed to be 1

private:
	std::string m_LastExitMessage;

	Core* m_CoreReference	{ nullptr };

	//Created or loaded later on
	GameObject* m_MainScene; 
	GameObject* m_ViewportCameraGO;
	Camera* m_ViewportCamera;

	//Temp
	GameObject* m_DirectionalLightTest;

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