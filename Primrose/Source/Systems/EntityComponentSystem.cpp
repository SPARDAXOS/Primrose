#include "Systems/EntityComponentSystem.hpp"
#include "Systems/Core.hpp"
#include "GameObject.hpp"


EntityComponentSystem::EntityComponentSystem(Core& core) noexcept
	:	m_CoreReference(&core)
{
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
	//TODO: Move into its own function as a special GO - Doesnt need to be in the gameobjects vector. move it out along with the scene and such! Latest Note!
	m_ViewportCameraGO = new GameObject(*this, VIEWPORT_CAMERA_OBJECT_ID);
	m_ViewportCameraGO->Awake(); //sure...
	m_ViewportCameraGO->SetName("ViewportCamera");
	m_GameObjects.push_back(m_ViewportCameraGO);
	m_ViewportCamera = m_ViewportCameraGO->AddComponent<Camera>();


	//This is temporary. Delete this after a more proper light sources are added.
	m_DirectionalLightTest = new GameObject(*this, 27);
	m_DirectionalLightTest->Awake();
	m_DirectionalLightTest->SetName("DirectionalLight");
	m_GameObjects.push_back(m_DirectionalLightTest);
	m_DirectionalLightTest->AddComponent<DirectionalLight>()->m_Tint = Colors::White;

}
EntityComponentSystem::~EntityComponentSystem() {
	//Clean all Gameobjects and Components
}

[[nodiscard]] bool EntityComponentSystem::Update() {

	for (auto& Object : m_GameObjects) {
		if (Object->GetEnabled() && Object->GetActiveInHeirarchy()) {
			if (!Object->GetStarted())
				Object->Start();
			if (!Object->GetStatic())
				CalculateTransformations(*Object);

			Object->Update();
		}
	}

	return true;
}

void EntityComponentSystem::CalculateTransformations(GameObject& object) {

	Transform* TargetTransform = &object.GetTransform();
	if (TargetTransform == nullptr)
		return;

	TargetTransform->ClearMatrix();
	TargetTransform->UpdateMatrix();

	GameObject* Parent = object.GetParent();
	if (Parent != nullptr) {
		TargetTransform->GetMatrix() = Parent->GetTransform().GetMatrix() * TargetTransform->GetMatrix();
	}
	if (object.HasChildren()) {
		for (auto& e : object.GetChildren())
			CalculateTransformations(*e);
	}
}


//TODO: Remove GameObject? make sure to adjust the mainscene children list too then!


GameObject& EntityComponentSystem::CreateGameObject() {
	//Creats one simply
	while (IsReserved(m_CurrentObjectIDIndex)) {
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
	while (IsReserved(m_CurrentObjectIDIndex)) {
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
	while (IsReserved(m_CurrentObjectIDIndex)) {
		m_CurrentObjectIDIndex++;
	}

	GameObject* NewGameObject = new GameObject(*this, m_CurrentObjectIDIndex);
	*NewGameObject = object;

	NewGameObject->Awake();
	m_GameObjects.push_back(NewGameObject);
	m_CurrentObjectIDIndex++;
	return *NewGameObject;
}

void EntityComponentSystem::DestroyGameObject(uint64 objectID) {

	for (uint32 index = 0; index < m_GameObjects.size(); index++) {
		if (m_GameObjects.at(index)->GetObjectID() == objectID) {
			m_MainScene->RemoveChild(m_GameObjects.at(index));
			delete m_GameObjects.at(index);
			m_GameObjects.erase(std::begin(m_GameObjects) + index);
		}
	}
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
	return INVALID_OBJECT_ID;
}
int32 EntityComponentSystem::FindCamera(uint64 objectID) const noexcept {
	for (uint32 index = 0; index < m_Cameras.size(); index++) {
		if (m_Cameras.at(index)->GetOwnerID() == objectID) {
			return index;
		}
	}
	return INVALID_OBJECT_ID;
}
int32 EntityComponentSystem::FindDirectionalLight(uint64 objectID) const noexcept {
	for (uint32 index = 0; index < m_DirectionalLights.size(); index++) {
		if (m_DirectionalLights.at(index)->GetOwnerID() == objectID) {
			return index;
		}
	}
	return INVALID_OBJECT_ID;
}
int32 EntityComponentSystem::FindPointLight(uint64 objectID) const noexcept {
	for (uint32 index = 0; index < m_PointLights.size(); index++) {
		if (m_PointLights.at(index)->GetOwnerID() == objectID) {
			return index;
		}
	}
	return INVALID_OBJECT_ID;
}

bool EntityComponentSystem::IsReserved(uint64 objectID) const noexcept {
	if (objectID == MAIN_SCENE_OBJECT_ID)
		return true;
	else if (objectID == VIEWPORT_CAMERA_OBJECT_ID)
		return true;

	return false;
}


//DELETE THESE AFTER PROPER LIGHT MANAGEMENT IS IMPLEMENTED!
GameObject* EntityComponentSystem::GetDirecitonalLightTEST() const noexcept { return m_DirectionalLightTest; }
GameObject* EntityComponentSystem::GetPointLightTEST() const noexcept { 

	if (m_PointLights.size() > 0)
		return m_PointLights.at(0)->GetOwner(); 

	return nullptr;
}