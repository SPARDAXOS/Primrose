#pragma once
#include "Utility.hpp"
#include "Systems/EntityComponentSystem.hpp"

class GameObject {
public:

	GameObject() = delete;
	GameObject(EntityComponentSystem& ecs, uint64 id)
		:	m_ECS(&ecs), m_ObjectID(id)
	{
	}
	virtual ~GameObject() {
		//Call ECS to clean up?
	}

	GameObject(const GameObject& other) noexcept {
		*this = other;
	}
	GameObject& operator=(const GameObject& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_ECS = other.m_ECS;
			this->m_Enabled = other.m_Enabled;
			this->m_Static = other.m_Static;
			this->m_Parent = other.m_Parent;
			this->m_ComponentFlags = other.m_ComponentFlags; //TODO: Make sure to check the flags and add the needed components by calling ECS
			this->m_ObjectName = other.m_ObjectName + " (Clone)";
			
			//Note: m_Started is not copyable or assignable

			return *this;
		}
	}

	GameObject(GameObject&& other) noexcept {
		*this = std::move(other);
	}
	GameObject& operator=(GameObject&& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_ECS = std::move(other.m_ECS);
			this->m_Enabled = std::move(other.m_Enabled);
			this->m_Static = std::move(other.m_Static);
			this->m_Parent = std::move(other.m_Parent);
			this->m_ComponentFlags = std::move(other.m_ComponentFlags); //TODO: Make sure to check the flags and COPY THE OWNERSHIP of the needed components by calling ECS
			this->m_ObjectName = std::move(other.m_ObjectName + " (Clone)");
			//hmm
			//Note: m_Started is not copyable or assignable
			return *this;
		}
	}

	bool operator==(const GameObject& rhs) const noexcept {
		if (&rhs == nullptr)
			return false;
		if (this->m_ObjectID == rhs.m_ObjectID)
			return true;
		return false;
	}

public:
	virtual void Awake() {
		
	}
	virtual void Start() {
		m_Started = true;

	}
	virtual void Update() {

	}


public:



	//TODO: Add support for getting and removing multiple components of type GameObject[] AddComponents();
	template<typename T>
	inline T* AddComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		if (HasComponent<T>())
			return nullptr;
		T* NewComponent = m_ECS->AddComponent<T>(m_ObjectID);
		if (NewComponent == nullptr)
			return nullptr;
		else {
			const uint32 ComponentID = Components::GetComponentID<T>();
			m_ComponentFlags |= ComponentID;
			return NewComponent;
		}
	}
	template<typename T>
	void RemoveComponent() noexcept {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		if (!HasComponent<T>())
			return;


		m_ECS->RemoveComponent<T>(m_ObjectID);
		const uint32 ComponentID = Components::GetComponentID<T>();
		m_ComponentFlags ^= ComponentID;
	}
	template<typename T>
	T* GetComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		if (!HasComponent<T>())
			return nullptr;
		
		return m_ECS->GetComponent<T>(m_ObjectID);
	}
	template<typename T>
	bool HasComponent() noexcept {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		const uint32 ComponentID = Components::GetComponentID<T>();
		if (ComponentID == INVALID_COMPONENT_ID)
			return false;

		uint32 Flags = m_ComponentFlags; //Using if ((m_ComponentFlags &= ComponentID) == 1) was actually clearing m_ComponentFlags!
		if ((Flags &= ComponentID) == ComponentID)
			return true;
		else
			return false;
	}

	void Destroy() {

		//Needs to be done for every components type
		if (HasComponent<SpriteRenderer>())
			RemoveComponent<SpriteRenderer>();
		if (HasComponent<Camera>())
			RemoveComponent<Camera>();

		if (HasChildren()) {
			for (auto& child : m_Children)
				child->Destroy();
		}

		if (m_Parent != nullptr)
			m_Parent->RemoveChild(this);

		m_ECS->DestroyGameObject(m_ObjectID);
	}

public:
	inline uint64 GetObjectID() const noexcept { return m_ObjectID; }
	inline std::string GetName() const noexcept { return m_ObjectName; }
	inline std::string GetTag() const noexcept { return m_ObjectTag; }

	inline bool GetEnabled() const noexcept { return m_Enabled; }
	inline bool GetActiveInHeirarchy() const noexcept { return m_ActiveInHeirarchy; }
	inline bool GetStatic() const noexcept { return m_Static; }
	inline bool GetStarted() const noexcept { return m_Started; }
	inline GameObject* GetParent() const noexcept { return m_Parent; }

	inline Transform& GetTransform() noexcept { return m_Transform; }

	inline void SetName(std::string name) noexcept { if (!name.empty()){ m_ObjectName = name; } }
	inline void SetTag(std::string tag) noexcept { m_ObjectTag = tag; }
	inline void SetEnabled(bool state) noexcept { 
		
		m_Enabled = state; 
		SetActiveInHeirarchy(state);
	}
	inline void SetActiveInHeirarchy(bool state) noexcept { 

		m_ActiveInHeirarchy = state; //Update this for unparented children and parented children
		if (HasChildren()) {
			for (auto& child : m_Children)
				child->SetActiveInHeirarchy(state);
		}
	}
	inline void SetStatic(bool state) noexcept { m_Static = state; }

public:
	bool AddChild(GameObject* child) noexcept;
	bool RemoveChild(GameObject* child) noexcept;
	inline bool HasChildren() const noexcept {
		if (m_Children.size() == 0)
			return false;
		return true;
	}
	GameObject* FindChild(std::string_view name) const noexcept;

	//FIX IDs for the heirarchy items having the same name and make sure that you cannot have items with the same name
	// -Loop through comparing names, if found then add index(1) to it and try again, if found agin then add +1 to the number and keep trying like thart
	//ActiveInHeirarchy for setting text color

	//propogate bool in set active maybe
	//set enabled sets acrive in hierarchy for self and all children
	//set active for heirarchy sets enablerd for self
	//Test filesystem by printing icons of every sprite in the texturestorage, at least its a test of the gui part of it

	inline std::vector<GameObject*> GetChildren() const noexcept {
		return m_Children;
	}

private:
	bool m_Enabled				{ true };
	bool m_ActiveInHeirarchy	{ true };
	bool m_Static				{ false }; 
	bool m_Started				{ false }; //For internal use only in ECS for calling Start()

private:
	Transform m_Transform;

private:
	GameObject* m_Parent{ nullptr };
	std::vector<GameObject*> m_Children;

private:
	EntityComponentSystem* m_ECS;
	uint32 m_ComponentFlags{ 0 };

private:
	uint64 m_ObjectID{ 0 };
	std::string m_ObjectName{ "NewGameObject" };
	std::string m_ObjectTag{ "None" };
};