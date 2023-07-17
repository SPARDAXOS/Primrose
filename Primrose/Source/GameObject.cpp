#include "GameObject.hpp"

void GameObject::SetName(std::string name) noexcept {
	if (!name.empty())
		m_ObjectName = name;
}
void GameObject::SetEnabled(bool state) noexcept {
	m_Enabled = state;
	
}

bool GameObject::AddChild(GameObject* child) noexcept {

	//Important Note: The camera might be fine but its the cursor reaching the edge that was locking it!

	if (child == nullptr)
		return false;

	//Case 3
	const GameObject* TargetChild = FindChild(child->GetName());
	if (TargetChild != nullptr) {
		RemoveChild(child);
		if (m_Parent != nullptr) {
			child->m_Parent = m_Parent;
			m_Parent->AddChild(child);
		}
		return true;
	}

	//Case 1
	if (*child == *m_Parent) {
		//Swap them!
		m_Parent = child->m_Parent;
		if (m_Parent != nullptr) { //If parents parent was not nullptr
			m_Parent->RemoveChild(child);
			m_Parent->AddChild(this);
		}

		child->RemoveChild(this);
		auto CachedChildren = this->GetChildren();
		auto SwapChildren = child->GetChildren();
		this->m_Children = SwapChildren;
		for (auto& e : m_Children)
			e->m_Parent = this;

		m_Children.push_back(child);

		child->m_Parent = this;
		child->m_Children = CachedChildren;
		for (auto& e : child->m_Children)
			e->m_Parent = child;
		
		return true;
	}
	else {
		//Case 2
		if (child->m_Parent != nullptr)
			child->m_Parent->RemoveChild(child);
		if (m_ObjectID != MAIN_SCENE_OBJECT_ID) //To not parent anything to the main scene
			child->m_Parent = this;
		m_Children.push_back(child);
		return true;
	}
}
bool GameObject::RemoveChild(GameObject* child) noexcept {

	if (child == nullptr)
		return false;

	for (uint32 i = 0; i < m_Children.size(); i++) {
		if (m_Children.at(i) == child) {
			if (child->m_Parent == this) {
				child->m_Parent = nullptr;
				child->GetTransform().m_Position += m_Transform.m_Position; //here!!!!!!!
			}
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