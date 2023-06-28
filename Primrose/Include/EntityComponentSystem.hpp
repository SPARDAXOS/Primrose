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


public:
	template<typename T>
	uint32 GetComponentsAmount() const noexcept;

	template<>
	uint32 GetComponentsAmount<SpriteRenderer>() const noexcept{
		return static_cast<uint32>(m_SpriteRenderers.size());
	}



public:
	inline GameObject& GetCurrentMainScene() const noexcept { return *m_MainScene; };

	//Can get all components of type. One by one after each call to function
	//Can create and return a reference to gameobject
	//Has the hierarchy with the scene component being the parent
	//Can create components and sorts them in lists
	//Can remove components
	//Can check for components using object ID
	//Keeps track of IDs

private:
	bool DoesGameObjectExist(uint64 ObjectID) const noexcept;

private:
	GameObject* FindGameObject(uint64 ObjectID) const noexcept;
	int32 FindSpriteRenderer(uint64 ObjectID) const noexcept;

private:
	std::vector<GameObject*> m_GameObjects;
	std::vector<SpriteRenderer*> m_SpriteRenderers;


	//Created or loaded later on
	GameObject* m_MainScene; 
	uint64 m_CurrentObjectIDIndex = 1;
};