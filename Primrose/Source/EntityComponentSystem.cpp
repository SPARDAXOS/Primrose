#include "EntityComponentSystem.hpp"
#include "GameObject.hpp"


EntityComponentSystem::EntityComponentSystem() noexcept {
	//Is this even necessary? I could just create a normal object using the function
	m_MainScene = new GameObject(*this, MAIN_SCENE_OBJECT_ID);
	m_MainScene->SetName("Scene");

	GameObject* MainCameraObject = &CreateGameObject("ViewportCamera"); //Does it need to be a gameobject? maybe cause the features are reusable?
	m_ViewportCamera = MainCameraObject->AddComponent<Camera>();
}
EntityComponentSystem::~EntityComponentSystem() {
	//Clean all Gameobjects and Components
}

[[nodiscard]] bool EntityComponentSystem::Update() const {

	for (auto& Object : m_GameObjects) {
		if (Object->GetEnabled()) {
			if (!Object->GetStarted())
				Object->Start();

			Object->Update();
		}
	}

	return true;
}


GameObject& EntityComponentSystem::CreateGameObject() {
	//Creats one simply
	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	NewGameObject->SetParent(m_MainScene);
	NewGameObject->Awake();
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}
GameObject& EntityComponentSystem::CreateGameObject(const std::string& name) {
	//Sets name as well
	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	NewGameObject->SetParent(m_MainScene);
	NewGameObject->SetName(name);
	NewGameObject->Awake();
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}
GameObject& EntityComponentSystem::Instantiate(const GameObject& object) {
	//Compies
	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	*NewGameObject = object;
	NewGameObject->Awake();
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
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
int32 EntityComponentSystem::FindCamera(uint64 objectID) const noexcept {
	for (uint32 index = 0; index < m_Cameras.size(); index++) {
		if (m_Cameras.at(index)->GetOwnerID() == objectID) {
			return index;
		}
	}
	return -1;
}