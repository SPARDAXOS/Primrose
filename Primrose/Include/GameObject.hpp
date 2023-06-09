#pragma once
#include "Utility.hpp"
#include "EntityComponentSystem.hpp"

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
		if (ComponentID == INVALID_ID)
			return false;

		if ((m_ComponentFlags &= ComponentID) == 1)
			return true;
		else
			return false;
	}

public:

	inline uint64 GetObjectID() const noexcept { return m_ObjectID; }
	inline std::string GetName() const noexcept { return m_ObjectName; }

	inline bool GetEnabled() const noexcept { return m_Enabled; }
	inline bool GetStarted() const noexcept { return m_Started; }
	inline GameObject* GetParent() const noexcept { return m_Parent; }

	inline Transform& GetTransform() noexcept { return m_Transform; }


	void SetName(std::string name) noexcept;
	void SetEnabled(bool state) noexcept;
	void SetParent(GameObject* parent) noexcept;


private:
	bool m_Enabled{ true };
	bool m_Static{ false }; //Not Implemented yet
	bool m_Started{ false };

private:
	Transform m_Transform;
	EntityComponentSystem* m_ECS;
	GameObject* m_Parent{ nullptr };
	uint32 m_ComponentFlags{ 0 };
	uint64 m_ObjectID{ 0 };
	std::string m_ObjectName{ "NewGameObject" };
};