#pragma once
#include "Component.hpp"
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
	

public:
	template<typename T>
	T* AddComponent(uint64 objectID);
	template<>
	SpriteRenderer* AddComponent<SpriteRenderer>(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, SpriteRenderer>); //Makes more sense for the custom components
		if (!DoesGameObjectExist(objectID))
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical


		SpriteRenderer* NewSpriteRenderer = new SpriteRenderer(objectID); //Add index in 
		m_SpriteRenderers.push_back(NewSpriteRenderer);
		return NewSpriteRenderer;
	}
	template<>
	Camera* AddComponent<Camera>(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, Camera>); //Makes more sense for the custom components
		if (!DoesGameObjectExist(objectID))
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical


		Camera* NewCamera = new Camera(objectID); //Add index in 
		m_Cameras.push_back(NewCamera);
		return NewCamera;
	}




	template<typename T>
	void RemoveComponent(uint64 objectID);
	template<>
	void RemoveComponent<SpriteRenderer>(uint64 objectID) noexcept {
		if (!DoesGameObjectExist(objectID))
			return;

		const int32 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == -1) //Maybe define some macro for this instead of -1
			return;

		m_SpriteRenderers.erase(std::begin(m_SpriteRenderers) + TargetIndex);
	}
	template<>
	void RemoveComponent<Camera>(uint64 objectID) noexcept {
		if (!DoesGameObjectExist(objectID))
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
		if (!DoesGameObjectExist(objectID))
			return nullptr;

		const int32 TargetIndex = FindSpriteRenderer(objectID);
		if (TargetIndex == -1) //Maybe define some macro for this instead of -1
			return nullptr;

		return m_SpriteRenderers.at(TargetIndex);
	}
	template<>
	Camera* GetComponent<Camera>(uint64 objectID) {
		if (!DoesGameObjectExist(objectID))
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

public:
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
	inline GameObject& GetCurrentScene() const noexcept { return *m_MainScene; };
	inline Camera& GetMainCamera() const noexcept { return *m_MainCamera; };

	GameObject* FindGameObject(uint64 ObjectID) const noexcept;


private:
	bool DoesGameObjectExist(uint64 objectID) const noexcept;

private:
	int32 FindSpriteRenderer(uint64 objectID) const noexcept;
	int32 FindCamera(uint64 objectID) const noexcept;

private:
	uint32  m_SpriteRenderersUpdateIndex = 0;


private:
	std::vector<GameObject*> m_GameObjects;
	std::vector<SpriteRenderer*> m_SpriteRenderers;
	std::vector<Camera*> m_Cameras;


	//Created or loaded later on
	GameObject* m_MainScene; 
	Camera* m_MainCamera;
	uint64 m_CurrentObjectIDIndex = 1;
};