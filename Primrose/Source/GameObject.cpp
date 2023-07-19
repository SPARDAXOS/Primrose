#include "GameObject.hpp"

bool GameObject::AddChild(GameObject* child) noexcept {

	//Important Note: The camera might be fine but its the cursor reaching the edge that was locking it!


	//NOTE: This is a confusing API. Rework this or at least restructure it so that it makes more sense.

	if (child == nullptr)
		return false;

	//Case 1 - This unparents the child from this gameobject
	const GameObject* TargetChild = FindChild(child->GetName());
	if (TargetChild != nullptr) {
		RemoveChild(child);
		if (m_Parent != nullptr) {
			child->m_Parent = m_Parent;
			m_Parent->AddChild(child);
		}
		else
			child->SetActiveInHeirarchy(child->GetEnabled());
		return true;
	}

	//Case 2 - This swaps the relationship between the child and parent 
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

	//Case 3 - This parents the child to this gameobject
	if (child->m_Parent != nullptr)
		child->m_Parent->RemoveChild(child);
	if (m_ObjectID != MAIN_SCENE_OBJECT_ID) //To not parent anything to the --MAIN-- scene
		child->m_Parent = this;
	child->SetEnabled(m_ActiveInHeirarchy); //Think about this thoroughly then rework the whole thing
	m_Children.push_back(child);

	return true;
}
bool GameObject::RemoveChild(GameObject* child) noexcept {

	if (child == nullptr)
		return false;

	for (uint32 i = 0; i < m_Children.size(); i++) {
		if (m_Children.at(i) == child) {
			if (child->m_Parent == this) {
				child->m_Parent = nullptr;

				//Keep getting parent transform and adding it
				//Basically cascading the transform from child to the last parent

				//In Adding, gotta make sure the object stays where it is in the world space
				//Multiplied by scale
				
				//TODO:Maybe make into an option? Perserve World Position?
				child->GetTransform().m_Position += m_Transform.m_Position * m_Transform.m_Scale; //here!!!!!!!
				child->GetTransform().m_Scale = child->GetTransform().m_Scale * m_Transform.m_Scale;
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