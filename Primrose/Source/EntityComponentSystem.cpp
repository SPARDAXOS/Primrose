#include "EntityComponentSystem.hpp"
#include "GameObject.hpp"


EntityComponentSystem::EntityComponentSystem() noexcept {
	m_MainScene = new GameObject(*this, MAIN_SCENE_OBJECT_ID);
	m_MainScene->SetName("Scene");
}
EntityComponentSystem::~EntityComponentSystem() {
	//Clean all Gameobjects and Components
}


GameObject& EntityComponentSystem::CreateGameObject() {
	//Creats one simply
	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}
GameObject& EntityComponentSystem::CreateGameObject(const std::string& name) {
	//Sets name as well
	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	NewGameObject->SetName(name);
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}
GameObject& EntityComponentSystem::Instantiate(const GameObject& object) {
	//Compies
	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	*NewGameObject = object;
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}

bool EntityComponentSystem::DoesGameObjectExist(uint64 id) const noexcept {
	for (auto& x : m_GameObjects) {
		if (x->GetObjectID() == id)
			return true;
	}
	return false;
}
GameObject* EntityComponentSystem::FindGameObject(uint64 id) const noexcept {
	for (auto& x : m_GameObjects) {
		if (x->GetObjectID() == id)
			return x;
	}
	return nullptr;
}