#include "EntityComponentSystem.hpp"
#include "GameObject.hpp"


EntityComponentSystem::EntityComponentSystem() noexcept {


	//TODO: Move these into functions that are called by the core or in intitialization if custom ctor is used

	//Is this even necessary? I could just create a normal object using the function
	m_MainScene = new GameObject(*this, MAIN_SCENE_OBJECT_ID);
	m_MainScene->Awake(); //sure...
	m_MainScene->SetName("Scene");


	//Viewport camera is special and should not be a child of the scene?
	//Regradless
	//BUG: the viewport camera has the same GO id as the scene for some reason. Test it thoroughly
	//GameObject* MainCameraObject = &CreateGameObject("ViewportCamera"); //Does it need to be a gameobject? maybe cause the features are reusable?


	//Hmmmm this makes the index thing more complicated and weird now..
	//Move into its own function as a special GO
	m_ViewportCameraGO = new GameObject(*this, VIEWPORT_CAMERA_OBJECT_ID);
	m_ViewportCameraGO->Awake(); //sure...
	m_ViewportCameraGO->SetName("ViewportCamera");
	m_GameObjects.push_back(m_ViewportCameraGO);
	m_ViewportCamera = m_ViewportCameraGO->AddComponent<Camera>();
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

//TODO: Remove GameObject? make sure to adjust the mainscene children list too then!


GameObject& EntityComponentSystem::CreateGameObject() {
	//Creats one simply
	while (!IsObjectIDAllowed(m_CurrentObjectIDIndex)) {
		m_CurrentObjectIDIndex++;
	}

	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	//NewGameObject->SetParent(m_MainScene);

	m_MainScene->AddChild(NewGameObject);
	NewGameObject->Awake();
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}
GameObject& EntityComponentSystem::CreateGameObject(const std::string& name) {
	//Sets name as well
	while (!IsObjectIDAllowed(m_CurrentObjectIDIndex)) {
		m_CurrentObjectIDIndex++;
	}

	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	//NewGameObject->SetParent(m_MainScene);

	m_MainScene->AddChild(NewGameObject);
	NewGameObject->SetName(name);
	NewGameObject->Awake();
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}
GameObject& EntityComponentSystem::Instantiate(const GameObject& object) {
	//Compies
	while (!IsObjectIDAllowed(m_CurrentObjectIDIndex)) {
		m_CurrentObjectIDIndex++;
	}

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
bool EntityComponentSystem::IsObjectIDAllowed(uint64 objectID) const noexcept {
	if (objectID == MAIN_SCENE_OBJECT_ID)
		return false;
	else if (objectID == VIEWPORT_CAMERA_OBJECT_ID)
		return false;

	return true;
}