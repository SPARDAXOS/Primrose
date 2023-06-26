#pragma once
#include "Component.hpp"
#include "GameObject.hpp"

class EntityComponentSystem final {
public:
	EntityComponentSystem() noexcept;
	~EntityComponentSystem();

	EntityComponentSystem(const EntityComponentSystem&) = delete;
	EntityComponentSystem& operator=(const EntityComponentSystem&) = delete;

	EntityComponentSystem(EntityComponentSystem&&) = delete;
	EntityComponentSystem& operator=(EntityComponentSystem&&) = delete;


	GameObject& CreateGameObject();
	GameObject& CreateGameObject(std::string& name);
	GameObject& Instantiate(GameObject& name);
	

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
	std::vector<GameObject*> m_GameObjects;


	//Created or loaded later on
	GameObject* m_MainScene; 

};