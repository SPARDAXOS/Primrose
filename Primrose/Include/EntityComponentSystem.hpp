#pragma once
#include "Components/SpriteRendererComponent.hpp"
#include "Components/CameraComponent.hpp"
#include <vector>

class GameObject;

class EntityComponentSystem final {
public:
	EntityComponentSystem() noexcept;
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




	template<typename T>
	void RemoveComponent(uint64 objectID);
	template<>
	void RemoveComponent<SpriteRenderer>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const int32 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == -1) //Maybe define some macro for this instead of -1
			return;

		m_SpriteRenderers.erase(std::begin(m_SpriteRenderers) + TargetIndex);
	}
	template<>
	void RemoveComponent<Camera>(uint64 objectID) noexcept {
		if (FindGameObject(objectID) == nullptr)
			return;

		const int32 TargetIndex = FindCamera(objectID);
		if (TargetIndex == -1) //Maybe define some macro for this instead of -1
			return;

		m_Cameras.erase(std::begin(m_Cameras) + TargetIndex);
	}




	template<typename T>
	T* GetComponent(uint64 objectID);
	template<>
	SpriteRenderer* GetComponent<SpriteRenderer>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const int32 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == -1) //Maybe define some macro for this instead of -1
			return nullptr;

		return m_SpriteRenderers.at(TargetIndex);
	}
	template<>
	Camera* GetComponent<Camera>(uint64 objectID) {
		if (FindGameObject(objectID) == nullptr)
			return nullptr;

		const int32 TargetIndex = FindCamera(objectID);
		if (TargetIndex == -1) //Maybe define some macro for this instead of -1
			return nullptr;

		return m_Cameras.at(TargetIndex);
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
	inline GameObject& GetCurrentScene() const noexcept { return *m_MainScene; };
	inline Camera& GetViewportCamera() const noexcept { return *m_ViewportCamera; };

	GameObject* FindGameObject(uint64 ObjectID) const noexcept;

private:
	void CalculateTransformations(GameObject& object);

private:
	int32 FindSpriteRenderer(uint64 objectID) const noexcept;
	int32 FindCamera(uint64 objectID) const noexcept;

	bool IsObjectIDAllowed(uint64 objectID) const noexcept;

private:
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; }

private:
	uint32  m_SpriteRenderersUpdateIndex = 0;


private:
	std::vector<GameObject*> m_GameObjects;
	std::vector<SpriteRenderer*> m_SpriteRenderers;
	std::vector<Camera*> m_Cameras;

private:
	std::string m_LastExitMessage;


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
	inline uint32 GetComponentID<SpriteRenderer>() noexcept { return SPRITE_COMPONENT_ID; }
	template<>
	inline uint32 GetComponentID<Camera>() noexcept { return CAMERA_COMPONENT_ID; }

	//TODO: Add Custom Component which is basically a component with no code that can be customized.
}