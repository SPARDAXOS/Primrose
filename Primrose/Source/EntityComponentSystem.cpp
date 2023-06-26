#include "EntityComponentSystem.hpp"


EntityComponentSystem::EntityComponentSystem() noexcept {
	m_MainScene = new GameObject(*this, MAIN_SCENE_OBJECT_ID);
	m_MainScene->SetObjectName("Scene");
}
EntityComponentSystem::~EntityComponentSystem() {

}


GameObject& EntityComponentSystem::CreateGameObject() {
	//Creats one simply
}
GameObject& EntityComponentSystem::CreateGameObject(std::string& name) {
	//Sets name as well
}
GameObject& EntityComponentSystem::Instantiate(GameObject& name) {
	//Compies
}