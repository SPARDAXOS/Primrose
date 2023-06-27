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
	NewGameObject->SetParent(m_MainScene);
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}
GameObject& EntityComponentSystem::CreateGameObject(const std::string& name) {
	//Sets name as well
	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	NewGameObject->SetParent(m_MainScene);
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

bool EntityComponentSystem::DoesGameObjectExist(uint64 objectID) const noexcept {
	for (auto& x : m_GameObjects) {
		if (x->GetObjectID() == objectID)
			return true;
	}
	return false;
}
GameObject* EntityComponentSystem::FindGameObject(uint64 objectID) const noexcept {
	for (auto& x : m_GameObjects) {
		if (x->GetObjectID() == objectID)
			return x;
	}
	return nullptr;
}

int32 EntityComponentSystem::FindSpriteRenderer(uint64 objectID) const noexcept {
	for (uint32 index = 0; index < m_SpriteRenderers.size(); index++) {
		if (m_SpriteRenderers.at(index)->GetOwnerID() == objectID) {
			return index;
		}
	}
	return -1;
}