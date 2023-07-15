#include "GameObject.hpp"

void GameObject::SetName(std::string name) noexcept {
	m_ObjectName = name;
}

void GameObject::SetEnabled(bool state) noexcept {
	m_Enabled = state;
	
}
//void GameObject::SetParent(GameObject* parent) noexcept {
//
//	if (m_Parent == parent)
//		return;
//
//
//	if (m_Parent == nullptr) {
//		//Add self to parent children
//		m_Parent = parent;
//		//Problem is, i cant explicitely edit the children list from here!
//		m_Parent->AddChild(this);
//		return;
//	}
//
//	if (m_Parent != nullptr) {
//		m_Parent->RemoveChild(this); //In case of replacing parent then remove self from old list
//		m_Parent = parent;
//		m_Parent->AddChild(this);
//		return;
//	}
//}
bool GameObject::AddChild(GameObject* child) noexcept {



	//Important Note: The camera might be fine but its the cursor reaching the edge that was locking it!

	if (child == nullptr)
		return false;

	const GameObject* TargetChild = FindChild(child->GetName());
	if (TargetChild != nullptr) //Child already exists
		return false;

	//Case 1
	if (child == m_Parent) {
		//Swap them!
		child->RemoveChild(this);
		m_Parent = child->m_Parent;
		child->m_Parent = this;
		m_Children.push_back(child);
		return true;
	}

	//Case 2
	if (child->m_Parent != nullptr)
		child->m_Parent->RemoveChild(child); //Should remove self from old parent list!
	if (m_ObjectID != MAIN_SCENE_OBJECT_ID) //To not parent anything to the main scene
		child->m_Parent = this; //This lets the scene end up as the parent. When i add the child to the scene this happens
	m_Children.push_back(child);
	return true;
}
bool GameObject::RemoveChild(GameObject* child) noexcept {

	if (child == nullptr)
		return false;

	for (uint32 i = 0; i < m_Children.size(); i++) {
		if (m_Children.at(i) == child) {
			child->m_Parent = nullptr;
			m_Children.erase(std::begin(m_Children) + i);
			return true;
		}
	}
	return false;
}
GameObject* GameObject::FindChild(std::string_view name) const noexcept {

	for (uint32 i = 0; i < m_Children.size(); i++) {
		if (m_Children.at(i)->GetName() == name) {
			return m_Children.at(i);
		}
	}
	return nullptr;
}