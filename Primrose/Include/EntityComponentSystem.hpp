#pragma once
#include "Component.hpp"

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
	T* AddComponent(uint64 objectID) {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		if (!DoesGameObjectExist(objectID))
			return nullptr;

		//Check limit on components? Maybe Gameobject side instead. one sounds logical

		//ERROR

		SpriteRenderer* NewSpriteRenderer = new SpriteRenderer(objectID); //Add index in 
		m_SpriteRenderers.push_back(NewSpriteRenderer);
		return NewSpriteRenderer;
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
	bool DoesGameObjectExist(uint64 id) const noexcept;
	GameObject* FindGameObject(uint64 id) const noexcept;

private:
	std::vector<GameObject*> m_GameObjects;
	std::vector<SpriteRenderer*> m_SpriteRenderers;


	//Created or loaded later on
	GameObject* m_MainScene; 
	uint64 m_CurrentObjectIDIndex = 1;
};