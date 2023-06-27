#include "GameObject.hpp"

void GameObject::SetName(std::string name) noexcept {
	m_ObjectName = name;
}

void GameObject::SetEnabled(bool state) noexcept {
	m_Enabled = state;
	
}
void GameObject::SetParent(GameObject* parent) noexcept {
	if (m_Parent->GetObjectID() == MAIN_SCENE_OBJECT_ID && parent == nullptr)
		return;
	if (m_Parent->GetObjectID() != MAIN_SCENE_OBJECT_ID && parent == nullptr) {
		m_Parent = &m_ECS->GetCurrentMainScene();
		return;
	}

	m_Parent = parent;
}